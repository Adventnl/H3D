#pragma once

// Menu: an ordered list of items, each bound to a command/operator id (or a
// separator). Menus are logical only — they carry no rendering or input logic.

#include <string>
#include <string_view>
#include <vector>

namespace forge::ui
{

struct MenuItem
{
    std::string label;
    std::string command_id;  // operator/command id invoked when chosen
    bool separator = false;
    bool enabled = true;

    [[nodiscard]] static MenuItem make_separator();
};

class Menu
{
public:
    explicit Menu(std::string title);

    [[nodiscard]] const std::string& title() const noexcept { return title_; }

    /// Append an item bound to a command id. Returns the item index.
    std::size_t add_item(std::string label, std::string command_id);
    void add_separator();

    [[nodiscard]] const std::vector<MenuItem>& items() const noexcept { return items_; }
    [[nodiscard]] std::size_t size() const noexcept { return items_.size(); }
    [[nodiscard]] bool empty() const noexcept { return items_.empty(); }

    /// Index of the first item bound to `command_id`, or -1 when absent.
    [[nodiscard]] int index_of_command(std::string_view command_id) const;

private:
    std::string title_;
    std::vector<MenuItem> items_;
};

} // namespace forge::ui
