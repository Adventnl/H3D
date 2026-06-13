#pragma once

// Window abstraction and the headless NullWindowBackend used by CI and tests.
//
// Phase 2 has no native OS window: a Window is a logical object with a size, a
// close flag and an event queue. A WindowBackend creates/destroys windows and
// pumps platform events; NullWindowBackend simulates this without any GUI
// dependency, so the whole shell runs headlessly. Native backends can be added
// later behind a build option.

#include <cstdint>
#include <deque>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "forge/app/event.hpp"

namespace forge
{

using WindowId = std::uint32_t;
inline constexpr WindowId kInvalidWindowId = 0;

struct WindowConfig
{
    std::string title = "Forge3D";
    int width = 1280;
    int height = 720;
    bool resizable = true;
    bool visible = true;
    bool headless = false;
};

class Window
{
public:
    Window(WindowId id, WindowConfig config);

    [[nodiscard]] WindowId id() const noexcept { return id_; }
    [[nodiscard]] const std::string& title() const noexcept { return config_.title; }
    void set_title(std::string title) { config_.title = std::move(title); }

    [[nodiscard]] int width() const noexcept { return width_; }
    [[nodiscard]] int height() const noexcept { return height_; }
    void set_size(int width, int height);

    [[nodiscard]] bool headless() const noexcept { return config_.headless; }
    [[nodiscard]] bool visible() const noexcept { return config_.visible; }

    [[nodiscard]] bool should_close() const noexcept { return should_close_; }
    void request_close() noexcept { should_close_ = true; }

    /// Inject an event into this window's queue (used by backends and tests).
    void queue_event(Event event);

    /// Pop the next queued event into `out`. Returns false when empty.
    bool poll_event(Event& out);

    [[nodiscard]] std::size_t pending_event_count() const noexcept { return events_.size(); }

private:
    WindowId id_;
    WindowConfig config_;
    int width_;
    int height_;
    bool should_close_ = false;
    std::deque<Event> events_;
};

/// Backend interface: how windows are created and how platform events flow in.
class WindowBackend
{
public:
    virtual ~WindowBackend();
    [[nodiscard]] virtual std::string_view name() const noexcept = 0;

    /// Pump platform events into the given windows. Returns the number of
    /// events produced this pump.
    virtual std::size_t pump_events(const std::vector<std::unique_ptr<Window>>& windows) = 0;
};

/// Headless backend: produces no events on its own; tests/callers inject events
/// directly via Window::queue_event. Always available, no GUI dependency.
class NullWindowBackend final : public WindowBackend
{
public:
    [[nodiscard]] std::string_view name() const noexcept override { return "null"; }
    std::size_t pump_events(const std::vector<std::unique_ptr<Window>>& windows) override;
};

class WindowSystem
{
public:
    /// Uses a NullWindowBackend when `backend` is null.
    explicit WindowSystem(std::unique_ptr<WindowBackend> backend = nullptr);

    [[nodiscard]] WindowBackend& backend() noexcept { return *backend_; }

    Window* create_window(const WindowConfig& config);
    bool destroy_window(WindowId id);
    [[nodiscard]] Window* find_window(WindowId id);

    /// Pump the backend, then drop any windows that requested close. Returns
    /// the number of events the backend produced.
    std::size_t poll_events();

    void request_quit() noexcept { quit_requested_ = true; }
    [[nodiscard]] bool quit_requested() const noexcept { return quit_requested_; }

    [[nodiscard]] bool has_windows() const noexcept { return !windows_.empty(); }
    [[nodiscard]] std::size_t window_count() const noexcept { return windows_.size(); }
    [[nodiscard]] Window* primary_window();

private:
    std::unique_ptr<WindowBackend> backend_;
    std::vector<std::unique_ptr<Window>> windows_;
    WindowId next_id_ = 1;
    bool quit_requested_ = false;
};

} // namespace forge
