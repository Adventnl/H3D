#include "forge/ui/screen.hpp"

#include <algorithm>
#include <utility>

namespace forge::ui
{

Screen::Screen(EditorType initial)
{
    auto area = std::make_unique<Area>(next_area_id_++, initial);
    area->ensure_default_regions();
    area->set_active(true);
    active_area_ = area->id();
    root_ = LayoutNode::make_leaf(std::move(area));
}

LayoutNode* Screen::find_leaf(LayoutNode* node, AreaId area_id)
{
    if (node == nullptr)
    {
        return nullptr;
    }
    if (node->is_leaf())
    {
        return node->area()->id() == area_id ? node : nullptr;
    }
    if (LayoutNode* found = find_leaf(node->first_.get(), area_id))
    {
        return found;
    }
    return find_leaf(node->second_.get(), area_id);
}

AreaId Screen::split_area(AreaId area_id, SplitDirection direction, float ratio,
                          std::optional<EditorType> new_editor)
{
    LayoutNode* leaf = find_leaf(root_.get(), area_id);
    if (leaf == nullptr)
    {
        return kInvalidAreaId;
    }

    const EditorType editor = new_editor.value_or(leaf->area()->editor_type());

    // Move the existing area into a new first-child leaf.
    auto existing = LayoutNode::make_leaf(std::move(leaf->area_));

    // Create the new area for the second-child leaf.
    auto new_area = std::make_unique<Area>(next_area_id_++, editor);
    new_area->ensure_default_regions();
    const AreaId new_id = new_area->id();
    auto created = LayoutNode::make_leaf(std::move(new_area));

    // Convert `leaf` in place from a leaf into a split node.
    leaf->area_.reset();
    leaf->direction_ = direction;
    leaf->ratio_ = std::clamp(ratio, 0.1f, 0.9f);
    leaf->first_ = std::move(existing);
    leaf->second_ = std::move(created);

    compute_layout(bounds_);
    return new_id;
}

bool Screen::remove_leaf(std::unique_ptr<LayoutNode>& owner, AreaId area_id)
{
    if (!owner || owner->is_leaf())
    {
        return false; // cannot remove the root leaf via this path
    }

    // If either direct child is the target leaf, replace `owner` with the other.
    const auto child_is_target = [area_id](const std::unique_ptr<LayoutNode>& child) {
        return child && child->is_leaf() && child->area()->id() == area_id;
    };

    if (child_is_target(owner->first_))
    {
        owner = std::move(owner->second_);
        return true;
    }
    if (child_is_target(owner->second_))
    {
        owner = std::move(owner->first_);
        return true;
    }

    return remove_leaf(owner->first_, area_id) || remove_leaf(owner->second_, area_id);
}

bool Screen::close_area(AreaId area_id)
{
    if (root_ && root_->is_leaf())
    {
        return false; // only one area left
    }
    if (find_leaf(root_.get(), area_id) == nullptr)
    {
        return false;
    }
    if (!remove_leaf(root_, area_id))
    {
        return false;
    }

    // Fix up the active area if it was the one closed.
    if (active_area_ == area_id)
    {
        std::vector<Area*> remaining = areas();
        active_area_ = remaining.empty() ? kInvalidAreaId : remaining.front()->id();
        for (Area* area : remaining)
        {
            area->set_active(area->id() == active_area_);
        }
    }
    compute_layout(bounds_);
    return true;
}

Area* Screen::find_area(AreaId area_id)
{
    LayoutNode* leaf = find_leaf(root_.get(), area_id);
    return leaf ? leaf->area() : nullptr;
}

const Area* Screen::find_area(AreaId area_id) const
{
    return const_cast<Screen*>(this)->find_area(area_id);
}

void Screen::collect(LayoutNode* node, std::vector<Area*>& out)
{
    if (node == nullptr)
    {
        return;
    }
    if (node->is_leaf())
    {
        out.push_back(node->area());
        return;
    }
    collect(node->first_.get(), out);
    collect(node->second_.get(), out);
}

void Screen::collect(const LayoutNode* node, std::vector<const Area*>& out) const
{
    if (node == nullptr)
    {
        return;
    }
    if (node->is_leaf())
    {
        out.push_back(node->area());
        return;
    }
    collect(node->first(), out);
    collect(node->second(), out);
}

std::vector<Area*> Screen::areas()
{
    std::vector<Area*> out;
    collect(root_.get(), out);
    return out;
}

std::vector<const Area*> Screen::areas() const
{
    std::vector<const Area*> out;
    collect(root_.get(), out);
    return out;
}

std::size_t Screen::area_count() const
{
    std::vector<const Area*> out;
    collect(root_.get(), out);
    return out.size();
}

bool Screen::set_area_editor_type(AreaId area_id, EditorType type)
{
    Area* area = find_area(area_id);
    if (area == nullptr)
    {
        return false;
    }
    area->set_editor_type(type);
    return true;
}

Area* Screen::active_area()
{
    return find_area(active_area_);
}

const Area* Screen::active_area() const
{
    return find_area(active_area_);
}

bool Screen::set_active_area(AreaId area_id)
{
    Area* target = find_area(area_id);
    if (target == nullptr)
    {
        return false;
    }
    for (Area* area : areas())
    {
        area->set_active(area->id() == area_id);
    }
    active_area_ = area_id;
    return true;
}

void Screen::layout_node(LayoutNode* node, const Rect& rect)
{
    if (node == nullptr)
    {
        return;
    }
    if (node->is_leaf())
    {
        node->area()->set_rect(rect);
        return;
    }

    if (node->direction() == SplitDirection::Vertical)
    {
        const float first_width = rect.width * node->ratio();
        layout_node(node->first_.get(), Rect{rect.x, rect.y, first_width, rect.height});
        layout_node(node->second_.get(),
                    Rect{rect.x + first_width, rect.y, rect.width - first_width,
                         rect.height});
    }
    else
    {
        const float first_height = rect.height * node->ratio();
        layout_node(node->first_.get(), Rect{rect.x, rect.y, rect.width, first_height});
        layout_node(node->second_.get(),
                    Rect{rect.x, rect.y + first_height, rect.width,
                         rect.height - first_height});
    }
}

void Screen::compute_layout(const Rect& bounds)
{
    bounds_ = bounds;
    layout_node(root_.get(), bounds);
}

} // namespace forge::ui
