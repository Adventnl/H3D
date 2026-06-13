#include "forge/app/window.hpp"

#include <algorithm>
#include <utility>

namespace forge
{

Window::Window(WindowId id, WindowConfig config)
    : id_(id), config_(std::move(config)), width_(config_.width), height_(config_.height)
{
}

void Window::set_size(int width, int height)
{
    width_ = std::max(0, width);
    height_ = std::max(0, height);
}

void Window::queue_event(Event event)
{
    events_.push_back(std::move(event));
}

bool Window::poll_event(Event& out)
{
    if (events_.empty())
    {
        return false;
    }
    out = std::move(events_.front());
    events_.pop_front();
    return true;
}

WindowBackend::~WindowBackend() = default;

std::size_t NullWindowBackend::pump_events(
    const std::vector<std::unique_ptr<Window>>& /*windows*/)
{
    // The headless backend produces no events on its own; callers/tests inject
    // events directly with Window::queue_event.
    return 0;
}

WindowSystem::WindowSystem(std::unique_ptr<WindowBackend> backend)
    : backend_(backend ? std::move(backend) : std::make_unique<NullWindowBackend>())
{
}

Window* WindowSystem::create_window(const WindowConfig& config)
{
    const WindowId id = next_id_++;
    windows_.push_back(std::make_unique<Window>(id, config));
    return windows_.back().get();
}

bool WindowSystem::destroy_window(WindowId id)
{
    const auto before = windows_.size();
    std::erase_if(windows_, [id](const std::unique_ptr<Window>& window) {
        return window->id() == id;
    });
    return windows_.size() != before;
}

Window* WindowSystem::find_window(WindowId id)
{
    for (const auto& window : windows_)
    {
        if (window->id() == id)
        {
            return window.get();
        }
    }
    return nullptr;
}

std::size_t WindowSystem::poll_events()
{
    const std::size_t produced = backend_->pump_events(windows_);
    // Drop windows that asked to close.
    std::erase_if(windows_, [](const std::unique_ptr<Window>& window) {
        return window->should_close();
    });
    return produced;
}

Window* WindowSystem::primary_window()
{
    return windows_.empty() ? nullptr : windows_.front().get();
}

} // namespace forge
