#pragma once

// Lightweight instrumentation profiler.
//
// begin_event/end_event pairs nest per thread (a thread-local stack tracks
// open events); completed events are appended to a shared, mutex-protected
// buffer. Timestamps are microseconds since the profiler was created, so
// traces from one run are directly comparable.
//
// Use the FORGE_PROFILE_* macros from profiler_scope.hpp in normal code.

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#include "forge/foundation/noncopyable.hpp"
#include "forge/foundation/time.hpp"

namespace forge
{

struct ProfileEvent
{
    std::string name;
    std::uint32_t thread_id = 0;
    std::int64_t start_microseconds = 0;    // relative to the profiler epoch
    std::int64_t duration_microseconds = 0;
};

class Profiler : NonMovable
{
public:
    [[nodiscard]] static Profiler& instance();

    void set_enabled(bool enabled) noexcept;
    [[nodiscard]] bool is_enabled() const noexcept;

    /// Open a named event on the calling thread. Must be balanced by
    /// end_event() on the same thread. No-op while disabled.
    void begin_event(std::string_view name);

    /// Close the innermost open event on the calling thread and record it.
    /// Ignored when there is no open event (e.g. disabled mid-scope).
    void end_event();

    /// Copy of all recorded events, oldest first.
    [[nodiscard]] std::vector<ProfileEvent> events() const;

    [[nodiscard]] std::size_t event_count() const;

    void clear();

    /// Microseconds elapsed since the profiler epoch.
    [[nodiscard]] std::int64_t now_microseconds() const noexcept;

private:
    Profiler();

    Instant epoch_;
    mutable std::mutex mutex_;
    std::vector<ProfileEvent> events_;
    std::atomic<bool> enabled_{true};
};

} // namespace forge
