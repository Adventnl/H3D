#include <atomic>
#include <cstdint>

#include "bench_framework.hpp"
#include "forge/threading/task_group.hpp"
#include "forge/threading/thread_pool.hpp"

using forge::TaskGroup;
using forge::ThreadPool;

FORGE_BENCHMARK("threading.thread_pool_tiny_tasks")
{
    // Measures scheduling overhead: many near-empty tasks.
    ThreadPool pool;
    TaskGroup group(pool);
    std::atomic<std::uint64_t> counter{0};

    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        group.run([&counter] { counter.fetch_add(1, std::memory_order_relaxed); });
    }
    group.wait();
    forge::bench::do_not_optimize(counter.load());
}

FORGE_BENCHMARK("threading.thread_pool_submit_future")
{
    // Measures the submit + future round trip on a single worker.
    ThreadPool pool(1);
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        auto future = pool.submit([index] { return index; });
        forge::bench::do_not_optimize(future.get());
    }
}
