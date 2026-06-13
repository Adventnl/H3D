#pragma once

// Widget: the base node of the logical UI tree. A widget has an id, bounds,
// visibility/enabled flags and child widgets. layout() positions children; the
// default is a no-op (children keep their own bounds). No drawing happens here.

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "forge/ui/rect.hpp"

namespace forge::ui
{

class Widget
{
public:
    explicit Widget(std::string id);
    virtual ~Widget();

    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;

    [[nodiscard]] const std::string& id() const noexcept { return id_; }

    [[nodiscard]] const Rect& bounds() const noexcept { return bounds_; }
    void set_bounds(const Rect& bounds) { bounds_ = bounds; }

    [[nodiscard]] bool visible() const noexcept { return visible_; }
    void set_visible(bool visible) { visible_ = visible; }

    [[nodiscard]] bool enabled() const noexcept { return enabled_; }
    void set_enabled(bool enabled) { enabled_ = enabled; }

    /// Append a child and return a non-owning pointer to it.
    Widget* add_child(std::unique_ptr<Widget> child);

    /// Remove the first child with the given id. Returns true if removed.
    bool remove_child(std::string_view id);

    [[nodiscard]] Widget* find_child(std::string_view id);
    [[nodiscard]] const Widget* find_child(std::string_view id) const;

    [[nodiscard]] std::size_t child_count() const noexcept { return children_.size(); }
    [[nodiscard]] const std::vector<std::unique_ptr<Widget>>& children() const noexcept
    {
        return children_;
    }
    void clear_children();

    /// Position children within this widget's bounds. Default: no-op. Calls
    /// layout() recursively on children after positioning.
    virtual void layout();

protected:
    Rect bounds_;

private:
    std::string id_;
    bool visible_ = true;
    bool enabled_ = true;
    std::vector<std::unique_ptr<Widget>> children_;
};

} // namespace forge::ui
