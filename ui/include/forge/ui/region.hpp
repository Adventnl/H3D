#pragma once

// Region: a sub-rectangle of an area with a fixed role (header, toolbar, the
// main content, etc.). An area's regions tile its rectangle.

#include <cstdint>
#include <string_view>

#include "forge/ui/rect.hpp"

namespace forge::ui
{

enum class RegionType : std::uint8_t
{
    Main = 0,   // central content
    Header,     // top strip (editor type selector, menus)
    Toolbar,    // tool column/row
    Sidebar,    // side panel (properties, n-panel)
    Footer,     // bottom strip
    Status,     // status line
    Count,
};

[[nodiscard]] std::string_view region_type_name(RegionType type) noexcept;

class Region
{
public:
    explicit Region(RegionType type, float size = 0.0f);

    [[nodiscard]] RegionType type() const noexcept { return type_; }

    [[nodiscard]] const Rect& rect() const noexcept { return rect_; }
    void set_rect(const Rect& rect) { rect_ = rect; }

    [[nodiscard]] bool visible() const noexcept { return visible_; }
    void set_visible(bool visible) { visible_ = visible; }

    /// Preferred thickness (height for horizontal regions, width for vertical).
    /// 0 means "take the remaining space" (used by the Main region).
    [[nodiscard]] float size() const noexcept { return size_; }
    void set_size(float size) { size_ = size; }

private:
    RegionType type_;
    Rect rect_;
    float size_;
    bool visible_ = true;
};

} // namespace forge::ui
