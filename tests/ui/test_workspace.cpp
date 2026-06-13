#include "test_framework.hpp"

#include "forge/ui/workspace.hpp"

using forge::ui::EditorType;

FORGE_TEST_CASE("ui.build_workspace_screen_single_editor")
{
    forge::ui::Screen screen = forge::ui::build_workspace_screen(EditorType::Viewport3D, {});
    FORGE_CHECK_EQ(screen.area_count(), static_cast<std::size_t>(1));
    FORGE_REQUIRE(screen.active_area() != nullptr);
    FORGE_CHECK(screen.active_area()->editor_type() == EditorType::Viewport3D);
}

FORGE_TEST_CASE("ui.build_workspace_screen_multiple_editors")
{
    forge::ui::Screen screen = forge::ui::build_workspace_screen(
        EditorType::Viewport3D,
        {EditorType::Outliner, EditorType::Properties, EditorType::Timeline});

    // One primary + three secondaries = four areas.
    FORGE_CHECK_EQ(screen.area_count(), static_cast<std::size_t>(4));

    // Every requested editor type appears at least once.
    bool has_viewport = false, has_outliner = false, has_props = false, has_timeline = false;
    for (const forge::ui::Area* area : screen.areas())
    {
        switch (area->editor_type())
        {
        case EditorType::Viewport3D: has_viewport = true; break;
        case EditorType::Outliner: has_outliner = true; break;
        case EditorType::Properties: has_props = true; break;
        case EditorType::Timeline: has_timeline = true; break;
        default: break;
        }
    }
    FORGE_CHECK(has_viewport && has_outliner && has_props && has_timeline);
}

FORGE_TEST_CASE("ui.workspace_holds_identity_and_screen")
{
    forge::ui::Screen screen = forge::ui::build_workspace_screen(EditorType::TextEditor,
                                                                {EditorType::PythonConsole});
    forge::ui::Workspace workspace("Scripting", "Scripting", std::move(screen),
                                   "Scripting workspace");
    FORGE_CHECK_EQ(workspace.id(), std::string("Scripting"));
    FORGE_CHECK_EQ(workspace.display_name(), std::string("Scripting"));
    FORGE_CHECK(workspace.screen().area_count() >= static_cast<std::size_t>(1));
}

FORGE_TEST_CASE("ui.workspace_screen_layout_is_valid")
{
    forge::ui::Screen screen = forge::ui::build_workspace_screen(
        EditorType::Viewport3D, {EditorType::Properties, EditorType::Outliner});
    screen.compute_layout(forge::ui::Rect{0.0f, 0.0f, 1280.0f, 720.0f});
    for (const forge::ui::Area* area : screen.areas())
    {
        FORGE_CHECK(area->rect().width > 0.0f);
        FORGE_CHECK(area->rect().height > 0.0f);
    }
}
