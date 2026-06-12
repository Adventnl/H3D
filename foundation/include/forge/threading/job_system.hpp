#pragma once

// Process-wide job system: a shared ThreadPool plus a parallel_for helper.
//
//   JobSystem::initialize();
//   JobSystem::parallel_for(0, count, [&](std::size_t i) { work(i); });
//   JobSystem::shutdown();
//
// initialize()/shutdown() are not thread-safe against concurrent job
// submission; call them from a single owning thread (the app entry point).
// parallel_for falls back to serial execution when the system is not
// initialized, so library code can use it unconditionally.

#include <algorithm>
#include <cstddef>
#include <utility>

#include "forge/threading/task_group.hpp"
#include "forge/threading/thread_pool.hpp"

namespace forge
{

class JobSystem
{
public:
    JobSystem() = delete;

    /// worker_count == 0 selects std::thread::hardware_concurrency().
    static void initialize(std::size_t worker_count = 0);

    /// Drains in-flight jobs and joins workers. Safe when not initialized.
    static void shutdown();

    [[nodiscard]] static bool is_initialized() noexcept;

    [[nodiscard]] static std::size_t worker_count() noexcept;

    /// The underlying pool. Asserts when not initialized.
    [[nodiscard]] static ThreadPool& pool();

    /// Fire-and-forget job.
    template <typename Callable>
    static void submit(Callable&& callable)
    {
        pool().submit(std::forward<Callable>(callable));
    }

    /// Invoke `function(index)` for every index in [begin, end), distributed
    /// in contiguous chunks across the workers. Blocks until done.
    /// `function` must be safe to call concurrently for distinct indices.
    template <typename Callable>
    static void parallel_for(std::size_t begin, std::size_t end, Callable&& function,
                             std::size_t min_chunk_size = 1)
    {
        if (begin >= end)
        {
            return;
        }
        const std::size_t count = end - begin;
        min_chunk_size = std::max<std::size_t>(min_chunk_size, 1);

        if (!is_initialized() || worker_count() <= 1 || count <= min_chunk_size)
        {
            for (std::size_t index = begin; index < end; ++index)
            {
                function(index);
            }
            return;
        }

        // ~4 chunks per worker balances load without excessive scheduling.
        const std::size_t target_chunks = worker_count() * 4;
        const std::size_t chunk_size =
            std::max(min_chunk_size, (count + target_chunks - 1) / target_chunks);

        TaskGroup group(pool());
        for (std::size_t chunk_begin = begin; chunk_begin < end; chunk_begin += chunk_size)
        {
            const std::size_t chunk_end = std::min(end, chunk_begin + chunk_size);
            group.run([chunk_begin, chunk_end, &function] {
                for (std::size_t index = chunk_begin; index < chunk_end; ++index)
                {
                    function(index);
                }
            });
        }
        group.wait();
    }
};

} // namespace forge
