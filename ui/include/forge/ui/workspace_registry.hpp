#pragma once

// WorkspaceRegistry: owns the workspaces and tracks the active one. It also
// implements forge::WorkspaceService so the app layer's workspace/screen
// operators (workspace.next, screen.split_*, ...) can act on it without the app
// module depending on ui.

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "forge/app/app_context.hpp"
#include "forge/ui/workspace.hpp"

namespace forge::ui
{

class WorkspaceRegistry final : public forge::WorkspaceService
{
public:
    WorkspaceRegistry();

    /// Append a workspace. The first registered workspace becomes active.
    Workspace& register_workspace(std::unique_ptr<Workspace> workspace);

    [[nodiscard]] Workspace* find(std::string_view id);
    [[nodiscard]] const Workspace* find(std::string_view id) const;
    [[nodiscard]] bool contains(std::string_view id) const;
    [[nodiscard]] std::size_t size() const noexcept { return workspaces_.size(); }
    [[nodiscard]] bool empty() const noexcept { return workspaces_.empty(); }

    [[nodiscard]] std::vector<Workspace*> list();
    [[nodiscard]] std::vector<std::string> ids() const;

    [[nodiscard]] Workspace* active();
    [[nodiscard]] const Workspace* active() const;
    [[nodiscard]] std::size_t active_index() const noexcept { return active_index_; }
    bool set_active_index(std::size_t index);

    // --- forge::WorkspaceService -----------------------------------------
    [[nodiscard]] std::string active_workspace_name() const override;
    bool next_workspace() override;
    bool previous_workspace() override;
    bool activate_workspace(const std::string& name) override;
    bool split_active_area_vertical() override;
    bool split_active_area_horizontal() override;
    bool close_active_area() override;

private:
    std::vector<std::unique_ptr<Workspace>> workspaces_;
    std::size_t active_index_ = 0;
};

/// Register the 16 default Blender-class workspaces and make "Layout" active.
void register_default_workspaces(WorkspaceRegistry& registry);

} // namespace forge::ui
