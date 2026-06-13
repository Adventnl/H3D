#include "forge/ui/area.hpp"

#include <algorithm>

namespace forge::ui
{

Area::Area(AreaId id, EditorType editor_type) : id_(id), editor_type_(editor_type) {}

void Area::set_rect(const Rect& rect)
{
    rect_ = rect;
    layout_regions();
}

Region& Area::add_region(RegionType type, float size)
{
    regions_.emplace_back(type, size);
    return regions_.back();
}

Region* Area::find_region(RegionType type)
{
    for (Region& region : regions_)
    {
        if (region.type() == type)
        {
            return &region;
        }
    }
    return nullptr;
}

const Region* Area::find_region(RegionType type) const
{
    for (const Region& region : regions_)
    {
        if (region.type() == type)
        {
            return &region;
        }
    }
    return nullptr;
}

void Area::ensure_default_regions()
{
    if (!regions_.empty())
    {
        return;
    }
    add_region(RegionType::Header, 26.0f);
    add_region(RegionType::Main, 0.0f); // 0 -> fills remaining space
    layout_regions();
}

void Area::layout_regions()
{
    // Carve fixed-thickness regions off the edges, leaving Main the remainder.
    Rect remaining = rect_;

    const auto carve_top = [&remaining](Region& region) {
        const float h = std::min(region.size(), remaining.height);
        region.set_rect(Rect{remaining.x, remaining.y, remaining.width, h});
        remaining.y += h;
        remaining.height -= h;
    };
    const auto carve_bottom = [&remaining](Region& region) {
        const float h = std::min(region.size(), remaining.height);
        region.set_rect(Rect{remaining.x, remaining.bottom() - h, remaining.width, h});
        remaining.height -= h;
    };
    const auto carve_left = [&remaining](Region& region) {
        const float w = std::min(region.size(), remaining.width);
        region.set_rect(Rect{remaining.x, remaining.y, w, remaining.height});
        remaining.x += w;
        remaining.width -= w;
    };

    // Order matters: top/bottom strips first, then side columns, then main.
    for (Region& region : regions_)
    {
        if (!region.visible() || region.type() != RegionType::Header)
        {
            continue;
        }
        carve_top(region);
    }
    for (Region& region : regions_)
    {
        if (!region.visible())
        {
            continue;
        }
        if (region.type() == RegionType::Footer || region.type() == RegionType::Status)
        {
            carve_bottom(region);
        }
    }
    for (Region& region : regions_)
    {
        if (region.visible() &&
            (region.type() == RegionType::Toolbar || region.type() == RegionType::Sidebar))
        {
            carve_left(region);
        }
    }
    for (Region& region : regions_)
    {
        if (region.visible() && region.type() == RegionType::Main)
        {
            region.set_rect(remaining);
        }
    }
}

} // namespace forge::ui
