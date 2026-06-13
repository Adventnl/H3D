#include "forge/ui/menu.hpp"

#include <utility>

namespace forge::ui
{

MenuItem MenuItem::make_separator()
{
    MenuItem item;
    item.separator = true;
    return item;
}

Menu::Menu(std::string title) : title_(std::move(title)) {}

std::size_t Menu::add_item(std::string label, std::string command_id)
{
    items_.push_back(MenuItem{std::move(label), std::move(command_id), false, true});
    return items_.size() - 1;
}

void Menu::add_separator()
{
    items_.push_back(MenuItem::make_separator());
}

int Menu::index_of_command(std::string_view command_id) const
{
    for (std::size_t i = 0; i < items_.size(); ++i)
    {
        if (!items_[i].separator && items_[i].command_id == command_id)
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

} // namespace forge::ui
