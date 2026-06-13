#include "forge/ui/widget.hpp"

#include <algorithm>
#include <utility>

namespace forge::ui
{

Widget::Widget(std::string id) : id_(std::move(id)) {}

Widget::~Widget() = default;

Widget* Widget::add_child(std::unique_ptr<Widget> child)
{
    if (!child)
    {
        return nullptr;
    }
    Widget* raw = child.get();
    children_.push_back(std::move(child));
    return raw;
}

bool Widget::remove_child(std::string_view id)
{
    const auto it = std::find_if(children_.begin(), children_.end(),
                                 [id](const std::unique_ptr<Widget>& child) {
                                     return child->id() == id;
                                 });
    if (it == children_.end())
    {
        return false;
    }
    children_.erase(it);
    return true;
}

Widget* Widget::find_child(std::string_view id)
{
    for (const auto& child : children_)
    {
        if (child->id() == id)
        {
            return child.get();
        }
    }
    return nullptr;
}

const Widget* Widget::find_child(std::string_view id) const
{
    for (const auto& child : children_)
    {
        if (child->id() == id)
        {
            return child.get();
        }
    }
    return nullptr;
}

void Widget::clear_children()
{
    children_.clear();
}

void Widget::layout()
{
    for (const auto& child : children_)
    {
        child->layout();
    }
}

} // namespace forge::ui
