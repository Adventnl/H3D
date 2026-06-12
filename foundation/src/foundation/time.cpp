#include "forge/foundation/time.hpp"

namespace forge
{

// ---------------------------------------------------------------------------
// Stopwatch
// ---------------------------------------------------------------------------
Stopwatch Stopwatch::start_new() noexcept
{
    Stopwatch stopwatch;
    stopwatch.start();
    return stopwatch;
}

void Stopwatch::start() noexcept
{
    if (!running_)
    {
        started_at_ = Instant::now();
        running_ = true;
    }
}

void Stopwatch::stop() noexcept
{
    if (running_)
    {
        accumulated_ += Instant::now() - started_at_;
        running_ = false;
    }
}

void Stopwatch::reset() noexcept
{
    accumulated_ = Duration{};
    running_ = false;
}

void Stopwatch::restart() noexcept
{
    accumulated_ = Duration{};
    started_at_ = Instant::now();
    running_ = true;
}

Duration Stopwatch::elapsed() const noexcept
{
    Duration total = accumulated_;
    if (running_)
    {
        total += Instant::now() - started_at_;
    }
    return total;
}

// ---------------------------------------------------------------------------
// FrameTimer
// ---------------------------------------------------------------------------
FrameTimer::FrameTimer() noexcept : start_(Instant::now()), last_tick_(start_) {}

Duration FrameTimer::tick() noexcept
{
    const Instant now = Instant::now();
    delta_ = now - last_tick_;
    last_tick_ = now;
    ++frame_count_;

    const double seconds = delta_.seconds();
    if (seconds > 0.0)
    {
        const double instantaneous_fps = 1.0 / seconds;
        // Exponential smoothing keeps the estimate stable across frame spikes.
        constexpr double smoothing = 0.1;
        smoothed_fps_ = (smoothed_fps_ == 0.0)
                            ? instantaneous_fps
                            : smoothed_fps_ + smoothing * (instantaneous_fps - smoothed_fps_);
    }
    return delta_;
}

} // namespace forge
