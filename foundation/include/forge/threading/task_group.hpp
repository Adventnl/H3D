#pragma once

// Groups related tasks on a ThreadPool and waits for all of them.
//
//   TaskGroup group(pool);
//   for (auto& item : items)
//       group.run([&item] { process(item); });
//   group.wait();
//
// Exceptions thrown by tasks are swallowed and counted in failed_count();
// the group always completes. Use ThreadPool::submit directly when you need
// per-task results or exceptions.

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <utility>

#include "forge/foundation/noncopyable.hpp"
#include "forge/threading/thread_pool.hpp"

namespace forge
{

class TaskGroup : NonMovable
{
public:
    explicit TaskGroup(ThreadPool& pool) : pool_(pool) {}

    /// Waits for any tasks still in flight.
    ~TaskGroup();

    template <typename Callable>
    void run(Callable&& callable)
    {
        pending_.fetch_add(1, std::memory_order_relaxed);
        pool_.submit(
            [this, task = std::forward<Callable>(callable)]() mutable {
                bool failed = false;
                try
                {
                    task();
                }
                catch (...)
                {
                    failed = true;
                }
                on_task_finished(failed);
            });
    }

    /// Block until every task added so far has finished.
    void wait();

    [[nodiscard]] std::size_t completed_count() const noexcept
    {
        return completed_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] std::size_t failed_count() const noexcept
    {
        return failed_.load(std::memory_order_relaxed);
    }

private:
    void on_task_finished(bool failed) noexcept;

    ThreadPool& pool_;
    std::atomic<std::size_t> pending_{0};
    std::atomic<std::size_t> completed_{0};
    std::atomic<std::size_t> failed_{0};
    std::mutex mutex_;
    std::condition_variable condition_;
};

} // namespace forge
