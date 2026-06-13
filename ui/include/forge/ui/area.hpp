#pragma once

// Area: a rectangular slot in a screen that hosts one editor. An area owns its
// regions (header, main, ...) and tiles them within its rectangle.

#include <cstdint>
#include <vector>

#include "forge/ui/editor_types.hpp"
#include "forge/ui/rect.hpp"
#include "forge/ui/region.hpp"

namespace forge::ui
{

using AreaId = std::uint32_t;
inline constexpr AreaId kInvalidAreaId = 0;

class Area
{
public:
    Area(AreaId id, EditorType editor_type);

    [[nodiscard]] AreaId id() const noexcept { return id_; }

    [[nodiscard]] EditorType editor_type() const noexcept { return editor_type_; }
    void set_editor_type(EditorType type) { editor_type_ = type; }

    [[nodiscard]] const Rect& rect() const noexcept { return rect_; }
    void set_rect(const Rect& rect);

    [[nodiscard]] const Size& min_size() const noexcept { return min_size_; }
    void set_min_size(const Size& size) { min_size_ = size; }

    [[nodiscard]] bool active() const noexcept { return active_; }
    void set_active(bool active) { active_ = active; }

    Region& add_region(RegionType type, float size = 0.0f);
    [[nodiscard]] Region* find_region(RegionType type);
    [[nodiscard]] const Region* find_region(RegionType type) const;
    [[nodiscard]] const std::vector<Region>& regions() const noexcept { return regions_; }
    [[nodiscard]] std::size_t region_count() const noexcept { return regions_.size(); }

    /// Give the area its standard regions (header + main) if it has none.
    void ensure_default_regions();

    /// Recompute region rectangles to tile the area rect: header on top, footer
    /// and status on the bottom, toolbar/sidebar on the sides, main fills the
    /// rest. Hidden regions are skipped and contribute no space.
    void layout_regions();

private:
    AreaId id_;
    EditorType editor_type_;
    Rect rect_;
    Size min_size_{120.0f, 80.0f};
    bool active_ = false;
    std::vector<Region> regions_;
};

} // namespace forge::ui
