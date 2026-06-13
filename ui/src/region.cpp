#include "forge/ui/region.hpp"

namespace forge::ui
{

std::string_view region_type_name(RegionType type) noexcept
{
    switch (type)
    {
    case RegionType::Main: return "Main";
    case RegionType::Header: return "Header";
    case RegionType::Toolbar: return "Toolbar";
    case RegionType::Sidebar: return "Sidebar";
    case RegionType::Footer: return "Footer";
    case RegionType::Status: return "Status";
    case RegionType::Count: return "Count";
    }
    return "Unknown";
}

Region::Region(RegionType type, float size) : type_(type), size_(size) {}

} // namespace forge::ui
