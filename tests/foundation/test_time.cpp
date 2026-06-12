#include <chrono>
#include <thread>

#include "forge/foundation/time.hpp"
#include "test_framework.hpp"

using forge::Duration;
using forge::FrameTimer;
using forge::Instant;
using forge::Stopwatch;

FORGE_TEST_CASE("foundation.duration_conversions")
{
    const Duration duration = Duration::from_milliseconds(1500);
    FORGE_CHECK_EQ(duration.milliseconds(), static_cast<std::int64_t>(1500));
    FORGE_CHECK_EQ(duration.microseconds(), static_cast<std::int64_t>(1'500'000));
    FORGE_CHECK_NEAR(duration.seconds(), 1.5, 1e-9);

    const Duration sum = duration + Duration::from_milliseconds(500);
    FORGE_CHECK_NEAR(sum.seconds(), 2.0, 1e-9);
    FORGE_CHECK(sum > duration);
}

FORGE_TEST_CASE("foundation.instant_elapsed_is_positive")
{
    const Instant start = Instant::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    const Duration elapsed = start.elapsed();
    FORGE_CHECK(elapsed.nanoseconds() > 0);
}

FORGE_TEST_CASE("foundation.stopwatch_accumulates")
{
    Stopwatch stopwatch = Stopwatch::start_new();
    FORGE_CHECK(stopwatch.is_running());
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    stopwatch.stop();
    const Duration first = stopwatch.elapsed();
    FORGE_CHECK(first.nanoseconds() > 0);

    // While stopped, elapsed time must not advance.
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    FORGE_CHECK_EQ(stopwatch.elapsed().nanoseconds(), first.nanoseconds());

    stopwatch.reset();
    FORGE_CHECK_EQ(stopwatch.elapsed().nanoseconds(), static_cast<std::int64_t>(0));
}

FORGE_TEST_CASE("foundation.frame_timer_ticks")
{
    FrameTimer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    const Duration delta = timer.tick();
    FORGE_CHECK(delta.nanoseconds() > 0);
    FORGE_CHECK_EQ(timer.frame_count(), static_cast<std::uint64_t>(1));
    FORGE_CHECK(timer.frames_per_second() > 0.0);
}
