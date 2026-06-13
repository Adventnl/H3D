#pragma once

// Screen: a Blender-style tiled layout of areas backed by a binary split tree
// (docking.hpp). A screen always has at least one area; the last area cannot be
// closed. compute_layout() turns the tree + an outer rect into concrete area
// and region rectangles.

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "forge/ui/area.hpp"
#include "forge/ui/docking.hpp"
#include "forge/ui/editor_types.hpp"
#include "forge/ui/rect.hpp"

namespace forge::ui
{

class Screen
{
public:
    /// A screen with a single full-size area hosting `initial` (active).
    explicit Screen(EditorType initial = EditorType::Viewport3D);

    Screen(const Screen&) = delete;
    Screen& operator=(const Screen&) = delete;
    Screen(Screen&&) noexcept = default;
    Screen& operator=(Screen&&) noexcept = default;

    /// Split the area with `area_id` into two. The original area keeps the
    /// first slot; a new area takes the second. When `new_editor` is empty the
    /// new area copies the original's editor type. Returns the new area's id,
    /// or kInvalidAreaId if `area_id` is unknown.
    AreaId split_area(AreaId area_id, SplitDirection direction, float ratio = 0.5f,
                      std::optional<EditorType> new_editor = std::nullopt);

    /// Close an area; its sibling expands to fill the freed space. Fails (false)
    /// when the area is unknown or it is the only area left.
    bool close_area(AreaId area_id);

    [[nodiscard]] Area* find_area(AreaId area_id);
    [[nodiscard]] const Area* find_area(AreaId area_id) const;
    [[nodiscard]] std::vector<Area*> areas();
    [[nodiscard]] std::vector<const Area*> areas() const;
    [[nodiscard]] std::size_t area_count() const;

    /// Change an area's hosted editor. Returns false if the area is unknown.
    bool set_area_editor_type(AreaId area_id, EditorType type);

    [[nodiscard]] AreaId active_area_id() const noexcept { return active_area_; }
    [[nodiscard]] Area* active_area();
    [[nodiscard]] const Area* active_area() const;
    bool set_active_area(AreaId area_id);

    /// Assign rectangles to every area (and its regions) for the given bounds.
    void compute_layout(const Rect& bounds);
    [[nodiscard]] const Rect& bounds() const noexcept { return bounds_; }

    [[nodiscard]] const LayoutNode* root() const noexcept { return root_.get(); }

private:
    LayoutNode* find_leaf(LayoutNode* node, AreaId area_id);
    bool remove_leaf(std::unique_ptr<LayoutNode>& owner, AreaId area_id);
    void collect(LayoutNode* node, std::vector<Area*>& out);
    void collect(const LayoutNode* node, std::vector<const Area*>& out) const;
    void layout_node(LayoutNode* node, const Rect& rect);

    std::unique_ptr<LayoutNode> root_;
    AreaId next_area_id_ = 1;
    AreaId active_area_ = kInvalidAreaId;
    Rect bounds_;
};

} // namespace forge::ui
