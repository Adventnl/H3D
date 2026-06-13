#include "forge/ui/workspace_registry.hpp"

#include <utility>

#include "forge/ui/docking.hpp"

namespace forge::ui
{

WorkspaceRegistry::WorkspaceRegistry() = default;

Workspace& WorkspaceRegistry::register_workspace(std::unique_ptr<Workspace> workspace)
{
    workspaces_.push_back(std::move(workspace));
    return *workspaces_.back();
}

Workspace* WorkspaceRegistry::find(std::string_view id)
{
    for (const auto& workspace : workspaces_)
    {
        if (workspace->id() == id)
        {
            return workspace.get();
        }
    }
    return nullptr;
}

const Workspace* WorkspaceRegistry::find(std::string_view id) const
{
    return const_cast<WorkspaceRegistry*>(this)->find(id);
}

bool WorkspaceRegistry::contains(std::string_view id) const
{
    return find(id) != nullptr;
}

std::vector<Workspace*> WorkspaceRegistry::list()
{
    std::vector<Workspace*> result;
    result.reserve(workspaces_.size());
    for (const auto& workspace : workspaces_)
    {
        result.push_back(workspace.get());
    }
    return result;
}

std::vector<std::string> WorkspaceRegistry::ids() const
{
    std::vector<std::string> result;
    result.reserve(workspaces_.size());
    for (const auto& workspace : workspaces_)
    {
        result.push_back(workspace->id());
    }
    return result;
}

Workspace* WorkspaceRegistry::active()
{
    if (workspaces_.empty())
    {
        return nullptr;
    }
    return workspaces_[active_index_].get();
}

const Workspace* WorkspaceRegistry::active() const
{
    return const_cast<WorkspaceRegistry*>(this)->active();
}

bool WorkspaceRegistry::set_active_index(std::size_t index)
{
    if (index >= workspaces_.size())
    {
        return false;
    }
    active_index_ = index;
    return true;
}

std::string WorkspaceRegistry::active_workspace_name() const
{
    const Workspace* workspace = active();
    return workspace ? workspace->display_name() : std::string{};
}

bool WorkspaceRegistry::next_workspace()
{
    if (workspaces_.size() < 2)
    {
        return false;
    }
    active_index_ = (active_index_ + 1) % workspaces_.size();
    return true;
}

bool WorkspaceRegistry::previous_workspace()
{
    if (workspaces_.size() < 2)
    {
        return false;
    }
    active_index_ = (active_index_ + workspaces_.size() - 1) % workspaces_.size();
    return true;
}

bool WorkspaceRegistry::activate_workspace(const std::string& name)
{
    for (std::size_t i = 0; i < workspaces_.size(); ++i)
    {
        if (workspaces_[i]->id() == name || workspaces_[i]->display_name() == name)
        {
            active_index_ = i;
            return true;
        }
    }
    return false;
}

bool WorkspaceRegistry::split_active_area_vertical()
{
    Workspace* workspace = active();
    if (workspace == nullptr)
    {
        return false;
    }
    Screen& screen = workspace->screen();
    return screen.split_area(screen.active_area_id(), SplitDirection::Vertical) !=
           kInvalidAreaId;
}

bool WorkspaceRegistry::split_active_area_horizontal()
{
    Workspace* workspace = active();
    if (workspace == nullptr)
    {
        return false;
    }
    Screen& screen = workspace->screen();
    return screen.split_area(screen.active_area_id(), SplitDirection::Horizontal) !=
           kInvalidAreaId;
}

bool WorkspaceRegistry::close_active_area()
{
    Workspace* workspace = active();
    if (workspace == nullptr)
    {
        return false;
    }
    Screen& screen = workspace->screen();
    return screen.close_area(screen.active_area_id());
}

namespace
{

std::unique_ptr<Workspace> make_workspace(std::string id, EditorType primary,
                                          const std::vector<EditorType>& secondaries,
                                          std::string description)
{
    Screen screen = build_workspace_screen(primary, secondaries);
    return std::make_unique<Workspace>(id, id, std::move(screen), std::move(description));
}

} // namespace

void register_default_workspaces(WorkspaceRegistry& registry)
{
    using ET = EditorType;

    registry.register_workspace(make_workspace(
        "Layout", ET::Viewport3D, {ET::Outliner, ET::Properties, ET::Timeline},
        "General-purpose scene layout."));
    registry.register_workspace(make_workspace(
        "Modeling", ET::Viewport3D, {ET::Outliner, ET::Properties}, "Mesh modeling."));
    registry.register_workspace(make_workspace(
        "Sculpting", ET::Viewport3D, {ET::Properties}, "Sculpting."));
    registry.register_workspace(make_workspace(
        "UV Editing", ET::Viewport3D, {ET::UVEditor, ET::ImageEditor, ET::Properties},
        "UV unwrapping."));
    registry.register_workspace(make_workspace(
        "Texture Paint", ET::Viewport3D, {ET::ImageEditor, ET::Properties},
        "Texture painting."));
    registry.register_workspace(make_workspace(
        "Shading", ET::Viewport3D, {ET::ShaderNodeEditor, ET::Properties},
        "Material shading."));
    registry.register_workspace(make_workspace(
        "Animation", ET::Viewport3D, {ET::Timeline, ET::DopeSheet, ET::GraphEditor},
        "Character and object animation."));
    registry.register_workspace(make_workspace(
        "Rendering", ET::Viewport3D, {ET::Properties, ET::Compositor}, "Rendering."));
    registry.register_workspace(make_workspace(
        "Compositing", ET::Compositor, {ET::ImageEditor}, "Image compositing."));
    registry.register_workspace(make_workspace(
        "Geometry Nodes", ET::Viewport3D,
        {ET::GeometryNodeEditor, ET::Spreadsheet, ET::Properties}, "Procedural geometry."));
    registry.register_workspace(make_workspace(
        "Scripting", ET::TextEditor, {ET::PythonConsole, ET::Properties}, "Scripting."));
    registry.register_workspace(make_workspace(
        "VFX", ET::MovieClipEditor, {ET::Viewport3D, ET::Properties},
        "Motion tracking and VFX."));
    registry.register_workspace(make_workspace(
        "Video Editing", ET::VideoSequencer, {ET::ImageEditor, ET::Properties},
        "Video sequence editing."));
    registry.register_workspace(make_workspace(
        "Grease Pencil", ET::Viewport3D, {ET::DopeSheet, ET::Properties},
        "2D/3D Grease Pencil drawing."));
    registry.register_workspace(make_workspace(
        "Assets", ET::AssetBrowser, {ET::Viewport3D, ET::Properties}, "Asset management."));
    registry.register_workspace(make_workspace(
        "Simulation", ET::Viewport3D, {ET::Properties, ET::Timeline}, "Physics simulation."));

    registry.activate_workspace("Layout");
}

} // namespace forge::ui
