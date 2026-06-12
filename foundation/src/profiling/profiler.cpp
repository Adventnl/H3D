#include "forge/profiling/profiler.hpp"

#include "forge/foundation/platform.hpp"

namespace forge
{
namespace
{

struct PendingEvent
{
    std::string name;
    std::int64_t start_microseconds = 0;
};

// Per-thread stack of open events; avoids any locking until an event closes.
thread_local std::vector<PendingEvent> t_open_events;

} // namespace

Profiler::Profiler() : epoch_(Instant::now()) {}

Profiler& Profiler::instance()
{
    static Profiler profiler;
    return profiler;
}

void Profiler::set_enabled(bool enabled) noexcept
{
    enabled_.store(enabled, std::memory_order_relaxed);
}

bool Profiler::is_enabled() const noexcept
{
    return enabled_.load(std::memory_order_relaxed);
}

std::int64_t Profiler::now_microseconds() const noexcept
{
    return epoch_.elapsed().microseconds();
}

void Profiler::begin_event(std::string_view name)
{
    if (!is_enabled())
    {
        return;
    }
    t_open_events.push_back(PendingEvent{std::string(name), now_microseconds()});
}

void Profiler::end_event()
{
    if (t_open_events.empty())
    {
        // begin_event was skipped (profiler disabled) or unbalanced; ignore.
        return;
    }

    PendingEvent pending = std::move(t_open_events.back());
    t_open_events.pop_back();

    ProfileEvent event;
    event.name = std::move(pending.name);
    event.thread_id = current_thread_id();
    event.start_microseconds = pending.start_microseconds;
    event.duration_microseconds = now_microseconds() - pending.start_microseconds;

    std::scoped_lock lock(mutex_);
    events_.push_back(std::move(event));
}

std::vector<ProfileEvent> Profiler::events() const
{
    std::scoped_lock lock(mutex_);
    return events_;
}

std::size_t Profiler::event_count() const
{
    std::scoped_lock lock(mutex_);
    return events_.size();
}

void Profiler::clear()
{
    std::scoped_lock lock(mutex_);
    events_.clear();
}

} // namespace forge
