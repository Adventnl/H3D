#include "forge/ui/toolbar.hpp"

#include <utility>

namespace forge::ui
{

Toolbar::Toolbar(std::string id) : id_(std::move(id)) {}

void Toolbar::add_tool(std::string tool_id, std::string label)
{
    tools_.push_back(ToolEntry{std::move(tool_id), std::move(label), true});
}

bool Toolbar::contains(std::string_view tool_id) const
{
    for (const ToolEntry& tool : tools_)
    {
        if (tool.id == tool_id)
        {
            return true;
        }
    }
    return false;
}

bool Toolbar::set_active_tool(std::string_view tool_id)
{
    if (!contains(tool_id))
    {
        return false;
    }
    active_tool_id_ = std::string(tool_id);
    return true;
}

} // namespace forge::ui
