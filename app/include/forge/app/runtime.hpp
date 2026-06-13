#pragma once

// Runtime: the per-process engine lifecycle and frame clock. It brings core
// subsystems up in order and tears them down in reverse, and tracks the frame
// index, wall-clock time, delta time and the quit request.
//
// Runtime deliberately owns very little: the Application wires the higher-level
// services together. Runtime is the heartbeat.

#include <cstdint>

#include "forge/foundation/result.hpp"
#include "forge/foundation/time.hpp"

namespace forge
{

class Runtime
{
public:
    Runtime() = default;
    ~Runtime();

    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;

    /// Bring subsystems up (currently the job system). Idempotent: a second
    /// call while already initialized is a no-op success.
    [[nodiscard]] Result<void> initialize();

    /// Tear subsystems down in reverse order. Safe to call when not
    /// initialized or more than once.
    void shutdown();

    [[nodiscard]] bool is_initialized() const noexcept { return initialized_; }

    /// Begin a frame: advances the frame index and computes delta time.
    void begin_frame();

    /// End a frame. Paired with begin_frame() for symmetry and future hooks.
    void end_frame();

    [[nodiscard]] std::uint64_t frame_index() const noexcept { return frame_index_; }

    /// Seconds elapsed since initialize().
    [[nodiscard]] double time() const noexcept;

    /// Seconds between the last two begin_frame() calls.
    [[nodiscard]] double delta_time() const noexcept { return delta_seconds_; }

    void request_quit() noexcept { quit_requested_ = true; }
    [[nodiscard]] bool quit_requested() const noexcept { return quit_requested_; }
    void clear_quit() noexcept { quit_requested_ = false; }

private:
    bool initialized_ = false;
    bool quit_requested_ = false;
    bool first_frame_ = true;
    std::uint64_t frame_index_ = 0;
    double delta_seconds_ = 0.0;
    Instant start_{};
    Instant last_frame_{};
};

} // namespace forge
