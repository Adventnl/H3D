#include "forge/ui/rect.hpp"

#include <algorithm>
#include <cmath>

namespace forge::ui
{

bool Rect::contains(float px, float py) const noexcept
{
    return px >= x && px < x + width && py >= y && py < y + height;
}

bool Rect::contains(Point point) const noexcept
{
    return contains(point.x, point.y);
}

bool Rect::intersects(const Rect& other) const noexcept
{
    return x < other.right() && right() > other.x && y < other.bottom() &&
           bottom() > other.y;
}

Rect Rect::split_left(float amount) const noexcept
{
    const float clamped = std::clamp(amount, 0.0f, width);
    return Rect{x, y, clamped, height};
}

Rect Rect::split_right(float amount) const noexcept
{
    const float clamped = std::clamp(amount, 0.0f, width);
    return Rect{x + width - clamped, y, clamped, height};
}

Rect Rect::split_top(float amount) const noexcept
{
    const float clamped = std::clamp(amount, 0.0f, height);
    return Rect{x, y, width, clamped};
}

Rect Rect::split_bottom(float amount) const noexcept
{
    const float clamped = std::clamp(amount, 0.0f, height);
    return Rect{x, y + height - clamped, width, clamped};
}

Rect Rect::inset(float margin) const noexcept
{
    const float new_width = std::max(0.0f, width - margin * 2.0f);
    const float new_height = std::max(0.0f, height - margin * 2.0f);
    return Rect{x + margin, y + margin, new_width, new_height};
}

bool Rect::operator==(const Rect& other) const noexcept
{
    const auto eq = [](float a, float b) { return std::fabs(a - b) <= 1e-4f; };
    return eq(x, other.x) && eq(y, other.y) && eq(width, other.width) &&
           eq(height, other.height);
}

} // namespace forge::ui
