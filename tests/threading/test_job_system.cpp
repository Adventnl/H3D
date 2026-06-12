#include <atomic>
#include <vector>

#include "forge/foundation/scope_exit.hpp"
#include "forge/threading/job_system.hpp"
#include "test_framework.hpp"

using forge::JobSystem;

FORGE_TEST_CASE("threading.job_system_initialize_shutdown")
{
    FORGE_CHECK(!JobSystem::is_initialized());

    JobSystem::initialize(2);
    FORGE_SCOPE_EXIT(JobSystem::shutdown());

    FORGE_CHECK(JobSystem::is_initialized());
    FORGE_CHECK_EQ(JobSystem::worker_count(), static_cast<std::size_t>(2));
}

FORGE_TEST_CASE("threading.job_system_parallel_for_visits_all_indices")
{
    JobSystem::initialize(4);
    FORGE_SCOPE_EXIT(JobSystem::shutdown());

    constexpr std::size_t kCount = 10'000;
    std::vector<std::atomic<int>> visits(kCount);

    JobSystem::parallel_for(0, kCount, [&visits](std::size_t index) {
        visits[index].fetch_add(1, std::memory_order_relaxed);
    });

    // Every index visited exactly once.
    bool all_visited_once = true;
    for (std::size_t index = 0; index < kCount; ++index)
    {
        if (visits[index].load(std::memory_order_relaxed) != 1)
        {
            all_visited_once = false;
            break;
        }
    }
    FORGE_CHECK(all_visited_once);
}

FORGE_TEST_CASE("threading.job_system_parallel_for_serial_fallback")
{
    // Without initialization, parallel_for must still do the work serially.
    FORGE_CHECK(!JobSystem::is_initialized());

    std::size_t sum = 0;
    JobSystem::parallel_for(1, 101, [&sum](std::size_t index) { sum += index; });
    FORGE_CHECK_EQ(sum, static_cast<std::size_t>(5050));
}

FORGE_TEST_CASE("threading.job_system_parallel_for_empty_range")
{
    int calls = 0;
    JobSystem::parallel_for(5, 5, [&calls](std::size_t) { ++calls; });
    JobSystem::parallel_for(10, 5, [&calls](std::size_t) { ++calls; });
    FORGE_CHECK_EQ(calls, 0);
}

FORGE_TEST_CASE("threading.job_system_submit")
{
    JobSystem::initialize(2);
    FORGE_SCOPE_EXIT(JobSystem::shutdown());

    std::atomic<bool> ran{false};
    JobSystem::submit([&ran] { ran.store(true); });

    // shutdown() drains pending jobs, so after scope exit the job has run.
    JobSystem::shutdown();
    FORGE_CHECK(ran.load());
}
