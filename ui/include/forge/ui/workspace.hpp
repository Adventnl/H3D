#pragma once

// Workspace: a named screen layout (e.g. "Layout", "Modeling"). Switching
// workspaces swaps the whole arrangement of editors the user sees.

#include <string>
#include <vector>

#include "forge/ui/editor_types.hpp"
#include "forge/ui/screen.hpp"

namespace forge::ui
{

class Workspace
{
public:
    Workspace(std::string id, std::string display_name, Screen screen,
              std::string description = {});

    [[nodiscard]] const std::string& id() const noexcept { return id_; }
    [[nodiscard]] const std::string& display_name() const noexcept { return display_name_; }
    [[nodiscard]] const std::string& description() const noexcept { return description_; }

    [[nodiscard]] Screen& screen() noexcept { return screen_; }
    [[nodiscard]] const Screen& screen() const noexcept { return screen_; }

private:
    std::string id_;
    std::string display_name_;
    std::string description_;
    Screen screen_;
};

/// Build a screen with `primary` filling the main slot and `secondaries`
/// stacked in a side column. Used to assemble the default workspaces.
[[nodiscard]] Screen build_workspace_screen(EditorType primary,
                                            const std::vector<EditorType>& secondaries);

} // namespace forge::ui
