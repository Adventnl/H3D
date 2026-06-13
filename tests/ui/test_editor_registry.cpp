#include "test_framework.hpp"

#include "forge/app/event.hpp"
#include "forge/ui/editor.hpp"
#include "forge/ui/editor_registry.hpp"
#include "forge/ui/editor_types.hpp"

using forge::ui::EditorRegistry;
using forge::ui::EditorType;

FORGE_TEST_CASE("ui.editor_registry_registers_all_defaults")
{
    EditorRegistry registry;
    forge::ui::register_default_editors(registry);
    FORGE_CHECK_EQ(registry.size(), forge::ui::kEditorTypeCount);
    FORGE_CHECK_EQ(registry.registered_types().size(), forge::ui::kEditorTypeCount);

    for (std::size_t i = 0; i < forge::ui::kEditorTypeCount; ++i)
    {
        FORGE_CHECK(registry.is_registered(static_cast<EditorType>(i)));
    }
}

FORGE_TEST_CASE("ui.editor_registry_create_by_type")
{
    EditorRegistry registry;
    forge::ui::register_default_editors(registry);

    auto viewport = registry.create(EditorType::Viewport3D);
    FORGE_REQUIRE(viewport != nullptr);
    FORGE_CHECK(viewport->type() == EditorType::Viewport3D);
    FORGE_CHECK(!viewport->display_name().empty());
    FORGE_CHECK(!viewport->status_text().empty());
}

FORGE_TEST_CASE("ui.editor_registry_unknown_create_returns_null")
{
    EditorRegistry registry; // nothing registered
    FORGE_CHECK(registry.create(EditorType::Outliner) == nullptr);
    FORGE_CHECK(!registry.is_registered(EditorType::Outliner));
}

FORGE_TEST_CASE("ui.editor_all_display_names_non_empty")
{
    EditorRegistry registry;
    forge::ui::register_default_editors(registry);
    for (const EditorType type : registry.registered_types())
    {
        auto editor = registry.create(type);
        FORGE_REQUIRE(editor != nullptr);
        FORGE_CHECK(!editor->display_name().empty());
    }
}

FORGE_TEST_CASE("ui.editor_update_and_event_are_safe")
{
    EditorRegistry registry;
    forge::ui::register_default_editors(registry);
    auto editor = registry.create(EditorType::Properties);
    FORGE_REQUIRE(editor != nullptr);

    forge::ui::EditorContext ctx; // app == nullptr
    editor->on_open(ctx);
    editor->update(ctx);
    FORGE_CHECK(!editor->handle_event(ctx, forge::Event::frame_begin()));
    editor->on_close(ctx);
}

FORGE_TEST_CASE("ui.editor_type_name_round_trip")
{
    FORGE_CHECK_EQ(forge::ui::editor_type_id(EditorType::Viewport3D),
                   std::string_view("viewport_3d"));
    FORGE_CHECK(forge::ui::editor_type_from_id("viewport_3d") == EditorType::Viewport3D);
    FORGE_CHECK(forge::ui::editor_type_from_id("nope") == EditorType::Count);
    FORGE_CHECK(!forge::ui::editor_type_name(EditorType::ShaderNodeEditor).empty());
}
