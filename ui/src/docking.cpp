#include "forge/ui/docking.hpp"

#include <utility>

namespace forge::ui
{

std::string_view split_direction_name(SplitDirection direction) noexcept
{
    switch (direction)
    {
    case SplitDirection::Vertical: return "Vertical";
    case SplitDirection::Horizontal: return "Horizontal";
    }
    return "Unknown";
}

std::unique_ptr<LayoutNode> LayoutNode::make_leaf(std::unique_ptr<Area> area)
{
    auto node = std::make_unique<LayoutNode>();
    node->area_ = std::move(area);
    return node;
}

} // namespace forge::ui
