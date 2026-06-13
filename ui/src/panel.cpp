#include "forge/ui/panel.hpp"

#include <utility>

namespace forge::ui
{

Panel::Panel(std::string id, std::string title)
    : Widget(std::move(id)), title_(std::move(title))
{
}

void Panel::layout()
{
    if (collapsed_)
    {
        // Header only; children are hidden but keep their last bounds.
        return;
    }

    // Stack visible children vertically below the header.
    float cursor_y = bounds_.y + header_height_;
    const float content_bottom = bounds_.bottom();
    for (const auto& child : children())
    {
        if (!child->visible())
        {
            continue;
        }
        const float child_height = child->bounds().height;
        Rect child_bounds{bounds_.x, cursor_y, bounds_.width, child_height};
        child->set_bounds(child_bounds);
        cursor_y += child_height;
        if (cursor_y > content_bottom)
        {
            cursor_y = content_bottom;
        }
    }

    Widget::layout();
}

} // namespace forge::ui
