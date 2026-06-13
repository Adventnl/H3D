#pragma once

// The docking/layout tree behind a Screen. A LayoutNode is either:
//   * a leaf, owning one Area, or
//   * a split, owning two child LayoutNodes divided by a ratio.
//
// SplitDirection names the divider:
//   Vertical   -> a vertical divider line -> left | right children
//   Horizontal -> a horizontal divider line -> top / bottom children
//
// The Screen owns the root node and performs structural edits (split / close),
// so it is a friend of LayoutNode. Outside the Screen, nodes are read-only.

#include <cstdint>
#include <memory>
#include <string_view>

#include "forge/ui/area.hpp"
#include "forge/ui/rect.hpp"

namespace forge::ui
{

enum class SplitDirection : std::uint8_t
{
    Vertical = 0,  // left | right
    Horizontal,    // top / bottom
};

[[nodiscard]] std::string_view split_direction_name(SplitDirection direction) noexcept;

class Screen; // owns and mutates the tree

class LayoutNode
{
public:
    [[nodiscard]] static std::unique_ptr<LayoutNode> make_leaf(std::unique_ptr<Area> area);

    [[nodiscard]] bool is_leaf() const noexcept { return area_ != nullptr; }

    [[nodiscard]] Area* area() noexcept { return area_.get(); }
    [[nodiscard]] const Area* area() const noexcept { return area_.get(); }

    [[nodiscard]] SplitDirection direction() const noexcept { return direction_; }
    [[nodiscard]] float ratio() const noexcept { return ratio_; }

    [[nodiscard]] LayoutNode* first() noexcept { return first_.get(); }
    [[nodiscard]] const LayoutNode* first() const noexcept { return first_.get(); }
    [[nodiscard]] LayoutNode* second() noexcept { return second_.get(); }
    [[nodiscard]] const LayoutNode* second() const noexcept { return second_.get(); }

private:
    friend class Screen;

    std::unique_ptr<Area> area_;                  // non-null => leaf
    SplitDirection direction_ = SplitDirection::Vertical;
    float ratio_ = 0.5f;                          // first child's fraction
    std::unique_ptr<LayoutNode> first_;
    std::unique_ptr<LayoutNode> second_;
};

} // namespace forge::ui
