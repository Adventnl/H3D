#pragma once

// Monotonic time utilities built on std::chrono::steady_clock.

#include <chrono>
#include <compare>
#include <cstdint>

namespace forge
{

/// A span of time with nanosecond resolution.
class Duration
{
public:
    constexpr Duration() = default;

    constexpr explicit Duration(std::chrono::nanoseconds value) : value_(value) {}

    [[nodiscard]] static constexpr Duration from_nanoseconds(std::int64_t nanoseconds)
    {
        return Duration(std::chrono::nanoseconds(nanoseconds));
    }

    [[nodiscard]] static constexpr Duration from_microseconds(std::int64_t microseconds)
    {
        return from_nanoseconds(microseconds * 1000);
    }

    [[nodiscard]] static constexpr Duration from_milliseconds(std::int64_t milliseconds)
    {
        return from_nanoseconds(milliseconds * 1'000'000);
    }

    [[nodiscard]] static constexpr Duration from_seconds(double seconds)
    {
        return from_nanoseconds(static_cast<std::int64_t>(seconds * 1e9));
    }

    [[nodiscard]] constexpr std::int64_t nanoseconds() const { return value_.count(); }
    [[nodiscard]] constexpr std::int64_t microseconds() const { return value_.count() / 1000; }
    [[nodiscard]] constexpr std::int64_t milliseconds() const { return value_.count() / 1'000'000; }
    [[nodiscard]] constexpr double seconds() const { return static_cast<double>(value_.count()) * 1e-9; }

    [[nodiscard]] constexpr std::chrono::nanoseconds to_chrono() const { return value_; }

    constexpr Duration& operator+=(Duration other)
    {
        value_ += other.value_;
        return *this;
    }

    constexpr Duration& operator-=(Duration other)
    {
        value_ -= other.value_;
        return *this;
    }

    [[nodiscard]] friend constexpr Duration operator+(Duration a, Duration b)
    {
        return Duration(a.value_ + b.value_);
    }

    [[nodiscard]] friend constexpr Duration operator-(Duration a, Duration b)
    {
        return Duration(a.value_ - b.value_);
    }

    [[nodiscard]] constexpr auto operator<=>(const Duration&) const = default;

private:
    std::chrono::nanoseconds value_{0};
};

/// A point on the monotonic clock.
class Instant
{
public:
    constexpr Instant() = default;

    [[nodiscard]] static Instant now() noexcept
    {
        return Instant(std::chrono::steady_clock::now());
    }

    /// Time elapsed between this instant and now.
    [[nodiscard]] Duration elapsed() const noexcept
    {
        return now() - *this;
    }

    [[nodiscard]] friend Duration operator-(Instant later, Instant earlier) noexcept
    {
        return Duration(std::chrono::duration_cast<std::chrono::nanoseconds>(
            later.time_point_ - earlier.time_point_));
    }

    [[nodiscard]] constexpr auto operator<=>(const Instant&) const = default;

private:
    constexpr explicit Instant(std::chrono::steady_clock::time_point time_point)
        : time_point_(time_point)
    {
    }

    std::chrono::steady_clock::time_point time_point_{};
};

/// Accumulating timer that can be started, stopped and reset.
class Stopwatch
{
public:
    Stopwatch() noexcept = default;

    /// A stopwatch that is already running.
    [[nodiscard]] static Stopwatch start_new() noexcept;

    void start() noexcept;
    void stop() noexcept;
    void reset() noexcept;
    void restart() noexcept;

    [[nodiscard]] bool is_running() const noexcept { return running_; }
    [[nodiscard]] Duration elapsed() const noexcept;

private:
    Duration accumulated_{};
    Instant started_at_{};
    bool running_ = false;
};

/// Tracks per-frame delta time and a smoothed frames-per-second estimate.
class FrameTimer
{
public:
    FrameTimer() noexcept;

    /// Marks the start of a new frame and returns the delta since the
    /// previous tick (or since construction for the first frame).
    Duration tick() noexcept;

    [[nodiscard]] Duration delta() const noexcept { return delta_; }
    [[nodiscard]] Duration total_time() const noexcept { return Instant::now() - start_; }
    [[nodiscard]] std::uint64_t frame_count() const noexcept { return frame_count_; }

    /// Exponentially smoothed FPS; 0 until the first tick.
    [[nodiscard]] double frames_per_second() const noexcept { return smoothed_fps_; }

private:
    Instant start_;
    Instant last_tick_;
    Duration delta_{};
    std::uint64_t frame_count_ = 0;
    double smoothed_fps_ = 0.0;
};

} // namespace forge
