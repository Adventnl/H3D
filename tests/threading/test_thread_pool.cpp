#include <atomic>
#include <chrono>
#include <future>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "forge/threading/task_group.hpp"
#include "forge/threading/thread_pool.hpp"
#include "test_framework.hpp"

using forge::TaskGroup;
using forge::ThreadPool;

FORGE_TEST_CASE("threading.thread_pool_executes_task")
{
    ThreadPool pool(2);
    std::atomic<bool> executed{false};
    pool.submit([&executed] { executed.store(true); }).wait();
    FORGE_CHECK(executed.load());
}

FORGE_TEST_CASE("threading.thread_pool_returns_value")
{
    ThreadPool pool(2);
    auto future = pool.submit([] { return 42; });
    FORGE_CHECK_EQ(future.get(), 42);

    // Arguments are forwarded to the callable.
    auto sum = pool.submit([](int a, int b) { return a + b; }, 20, 22);
    FORGE_CHECK_EQ(sum.get(), 42);

    auto text = pool.submit([] { return std::string("forge"); });
    FORGE_CHECK_EQ(text.get(), std::string("forge"));
}

FORGE_TEST_CASE("threading.thread_pool_completes_many_tasks")
{
    ThreadPool pool(4);
    constexpr int kTaskCount = 1000;

    std::atomic<int> counter{0};
    std::vector<std::future<void>> futures;
    futures.reserve(kTaskCount);
    for (int index = 0; index < kTaskCount; ++index)
    {
        futures.push_back(pool.submit([&counter] { counter.fetch_add(1); }));
    }
    for (auto& future : futures)
    {
        future.get();
    }
    FORGE_CHECK_EQ(counter.load(), kTaskCount);
}

FORGE_TEST_CASE("threading.thread_pool_propagates_exceptions")
{
    ThreadPool pool(1);
    auto future = pool.submit([]() -> int { throw std::runtime_error("task failed"); });

    bool caught = false;
    try
    {
        future.get();
    }
    catch (const std::runtime_error& error)
    {
        caught = std::string(error.what()) == "task failed";
    }
    FORGE_CHECK(caught);
}

FORGE_TEST_CASE("threading.thread_pool_drains_queue_on_shutdown")
{
    std::atomic<int> counter{0};
    {
        ThreadPool pool(2);
        for (int index = 0; index < 100; ++index)
        {
            pool.submit([&counter] {
                std::this_thread::sleep_for(std::chrono::microseconds(50));
                counter.fetch_add(1);
            });
        }
        // Destructor shuts down; every queued task must still run.
    }
    FORGE_CHECK_EQ(counter.load(), 100);
}

FORGE_TEST_CASE("threading.thread_pool_worker_count")
{
    ThreadPool pool(3);
    FORGE_CHECK_EQ(pool.worker_count(), static_cast<std::size_t>(3));
    FORGE_CHECK(pool.is_running());

    pool.shutdown();
    FORGE_CHECK(!pool.is_running());

    // Submitting after shutdown still satisfies the future (runs inline).
    auto future = pool.submit([] { return 7; });
    FORGE_CHECK_EQ(future.get(), 7);
}

FORGE_TEST_CASE("threading.task_group_waits_for_all")
{
    ThreadPool pool(4);
    TaskGroup group(pool);

    std::atomic<int> counter{0};
    constexpr int kTaskCount = 64;
    for (int index = 0; index < kTaskCount; ++index)
    {
        group.run([&counter] { counter.fetch_add(1); });
    }
    group.wait();

    FORGE_CHECK_EQ(counter.load(), kTaskCount);
    FORGE_CHECK_EQ(group.completed_count(), static_cast<std::size_t>(kTaskCount));
    FORGE_CHECK_EQ(group.failed_count(), static_cast<std::size_t>(0));
}

FORGE_TEST_CASE("threading.task_group_counts_failures")
{
    ThreadPool pool(2);
    TaskGroup group(pool);

    group.run([] { throw std::runtime_error("intentional"); });
    group.run([] {});
    group.wait();

    FORGE_CHECK_EQ(group.completed_count(), static_cast<std::size_t>(2));
    FORGE_CHECK_EQ(group.failed_count(), static_cast<std::size_t>(1));
}
