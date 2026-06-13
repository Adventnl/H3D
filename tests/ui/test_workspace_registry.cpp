#include "test_framework.hpp"

#include "forge/ui/workspace_registry.hpp"

FORGE_TEST_CASE("ui.workspace_registry_registers_all_defaults")
{
    forge::ui::WorkspaceRegistry registry;
    forge::ui::register_default_workspaces(registry);
    FORGE_CHECK_EQ(registry.size(), static_cast<std::size_t>(16));

    // Spot-check that the Blender-class workspace names exist.
    FORGE_CHECK(registry.contains("Layout"));
    FORGE_CHECK(registry.contains("Modeling"));
    FORGE_CHECK(registry.contains("Sculpting"));
    FORGE_CHECK(registry.contains("UV Editing"));
    FORGE_CHECK(registry.contains("Shading"));
    FORGE_CHECK(registry.contains("Animation"));
    FORGE_CHECK(registry.contains("Geometry Nodes"));
    FORGE_CHECK(registry.contains("Scripting"));
    FORGE_CHECK(registry.contains("Simulation"));
}

FORGE_TEST_CASE("ui.workspace_registry_active_defaults_to_layout")
{
    forge::ui::WorkspaceRegistry registry;
    forge::ui::register_default_workspaces(registry);
    FORGE_REQUIRE(registry.active() != nullptr);
    FORGE_CHECK_EQ(registry.active_workspace_name(), std::string("Layout"));
}

FORGE_TEST_CASE("ui.workspace_registry_next_previous")
{
    forge::ui::WorkspaceRegistry registry;
    forge::ui::register_default_workspaces(registry);

    const std::string first = registry.active_workspace_name();
    FORGE_CHECK(registry.next_workspace());
    FORGE_CHECK(registry.active_workspace_name() != first);
    FORGE_CHECK(registry.previous_workspace());
    FORGE_CHECK_EQ(registry.active_workspace_name(), first);

    // Wrap-around: previous from the first goes to the last.
    FORGE_CHECK(registry.previous_workspace());
    FORGE_CHECK_EQ(registry.active_index(), registry.size() - 1);
}

FORGE_TEST_CASE("ui.workspace_registry_activate_by_name")
{
    forge::ui::WorkspaceRegistry registry;
    forge::ui::register_default_workspaces(registry);
    FORGE_CHECK(registry.activate_workspace("Animation"));
    FORGE_CHECK_EQ(registry.active_workspace_name(), std::string("Animation"));
    FORGE_CHECK(!registry.activate_workspace("DoesNotExist"));
}

FORGE_TEST_CASE("ui.workspace_registry_every_workspace_has_areas")
{
    forge::ui::WorkspaceRegistry registry;
    forge::ui::register_default_workspaces(registry);
    for (const forge::ui::Workspace* workspace : registry.list())
    {
        FORGE_CHECK(workspace->screen().area_count() >= static_cast<std::size_t>(1));
    }
}

FORGE_TEST_CASE("ui.workspace_registry_implements_workspace_service")
{
    forge::ui::WorkspaceRegistry registry;
    forge::ui::register_default_workspaces(registry);

    // Drive it through the app-layer interface.
    forge::WorkspaceService& service = registry;
    const std::string before = service.active_workspace_name();
    FORGE_CHECK(service.next_workspace());
    FORGE_CHECK(service.active_workspace_name() != before);

    // Split the active area through the service.
    forge::ui::Workspace* active = registry.active();
    FORGE_REQUIRE(active != nullptr);
    const std::size_t areas_before = active->screen().area_count();
    FORGE_CHECK(service.split_active_area_vertical());
    FORGE_CHECK_EQ(active->screen().area_count(), areas_before + 1);

    // Close it again.
    FORGE_CHECK(service.close_active_area());
}
