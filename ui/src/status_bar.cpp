#include "forge/ui/status_bar.hpp"

#include <algorithm>
#include <utility>

namespace forge::ui
{

void StatusBar::set_message(std::string message)
{
    message_ = std::move(message);
}

void StatusBar::push_message(std::string message)
{
    message_ = message;
    history_.push_back(std::move(message));
    while (history_limit_ != 0 && history_.size() > history_limit_)
    {
        history_.erase(history_.begin());
    }
}

void StatusBar::clear()
{
    message_.clear();
    history_.clear();
    progress_ = -1.0f;
}

void StatusBar::set_progress(float progress)
{
    progress_ = std::clamp(progress, 0.0f, 1.0f);
}

void StatusBar::set_history_limit(std::size_t limit)
{
    history_limit_ = limit;
    while (history_limit_ != 0 && history_.size() > history_limit_)
    {
        history_.erase(history_.begin());
    }
}

} // namespace forge::ui
