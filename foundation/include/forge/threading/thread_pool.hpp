#pragma once

// Fixed-size worker pool with a mutex-protected FIFO queue.
//
//   ThreadPool pool(4);
//   auto future = pool.submit([] { return 42; });
//   int value = future.get();
//
// Shutdown is graceful: queued tasks are drained before workers exit, so no
// submitted task is lost. Tasks submitted after shutdown run inline on the
// calling thread (documented fallback rather than silent loss).

#include <concepts>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "forge/foundation/assert.hpp"
#include "forge/foundation/noncopyable.hpp"

namespace forge
{

/// Move-only type-erased callable; std::function requires copyability,
/// which std::packaged_task does not provide.
class UniqueFunction
{
public:
    UniqueFunction() noexcept = default;

    template <typename Callable>
        requires(!std::same_as<std::remove_cvref_t<Callable>, UniqueFunction>) &&
                std::invocable<std::decay_t<Callable>&>
    UniqueFunction(Callable&& callable)
        : callable_(std::make_unique<Model<std::decay_t<Callable>>>(
              std::forward<Callable>(callable)))
    {
    }

    UniqueFunction(UniqueFunction&&) noexcept = default;
    UniqueFunction& operator=(UniqueFunction&&) noexcept = default;

    void operator()()
    {
        FORGE_ASSERT(callable_ != nullptr);
        callable_->invoke();
    }

    [[nodiscard]] explicit operator bool() const noexcept { return callable_ != nullptr; }

private:
    struct CallableBase
    {
        virtual ~CallableBase() = default;
        virtual void invoke() = 0;
    };

    template <typename Callable>
    struct Model final : CallableBase
    {
        explicit Model(Callable wrapped) : callable_(std::move(wrapped)) {}
        void invoke() override { callable_(); }
        Callable callable_;
    };

    std::unique_ptr<CallableBase> callable_;
};

class ThreadPool : NonMovable
{
public:
    /// worker_count == 0 selects std::thread::hardware_concurrency().
    explicit ThreadPool(std::size_t worker_count = 0);

    /// Joins all workers after draining the queue.
    ~ThreadPool();

    /// Schedule a callable; the returned future carries the result (or the
    /// exception the callable threw).
    template <typename Callable, typename... Args>
    auto submit(Callable&& callable, Args&&... args)
        -> std::future<std::invoke_result_t<std::decay_t<Callable>, std::decay_t<Args>...>>
    {
        using ResultType =
            std::invoke_result_t<std::decay_t<Callable>, std::decay_t<Args>...>;

        std::packaged_task<ResultType()> task(
            [function = std::forward<Callable>(callable),
             ... arguments = std::forward<Args>(args)]() mutable -> ResultType {
                return std::invoke(std::move(function), std::move(arguments)...);
            });
        std::future<ResultType> future = task.get_future();

        UniqueFunction job(std::move(task));
        if (!try_enqueue(std::move(job)))
        {
            // Pool already shut down: run inline so the future is still
            // satisfied and no work is dropped.
            job();
        }
        return future;
    }

    /// Drain the queue and join all workers. Safe to call more than once.
    void shutdown();

    [[nodiscard]] std::size_t worker_count() const noexcept { return workers_.size(); }
    [[nodiscard]] std::size_t pending_count() const;
    [[nodiscard]] bool is_running() const;

private:
    void worker_loop();

    /// Returns false (without consuming the job) when the pool is stopping.
    [[nodiscard]] bool try_enqueue(UniqueFunction&& job);

    std::vector<std::thread> workers_;
    std::deque<UniqueFunction> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    bool stopping_ = false;
};

} // namespace forge
