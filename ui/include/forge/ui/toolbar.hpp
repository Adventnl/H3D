#pragma once

// Toolbar: a row of tool entries with one active tool. A tool entry carries an
// id (the operator/tool it activates), a label and an enabled flag.

#include <string>
#include <string_view>
#include <vector>

namespace forge::ui
{

struct ToolEntry
{
    std::string id;
    std::string label;
    bool enabled = true;
};

class Toolbar
{
public:
    explicit Toolbar(std::string id);

    [[nodiscard]] const std::string& id() const noexcept { return id_; }

    void add_tool(std::string tool_id, std::string label);
    [[nodiscard]] const std::vector<ToolEntry>& tools() const noexcept { return tools_; }
    [[nodiscard]] std::size_t size() const noexcept { return tools_.size(); }
    [[nodiscard]] bool contains(std::string_view tool_id) const;

    /// Set the active tool. Returns false if the id is not in this toolbar.
    bool set_active_tool(std::string_view tool_id);
    [[nodiscard]] const std::string& active_tool_id() const noexcept { return active_tool_id_; }
    void clear_active_tool() { active_tool_id_.clear(); }

private:
    std::string id_;
    std::vector<ToolEntry> tools_;
    std::string active_tool_id_;
};

} // namespace forge::ui
