#include "test_framework.hpp"

#include <memory>

#include "forge/ui/menu.hpp"
#include "forge/ui/panel.hpp"
#include "forge/ui/status_bar.hpp"
#include "forge/ui/toolbar.hpp"
#include "forge/ui/widget.hpp"

FORGE_TEST_CASE("ui.widget_child_add_remove")
{
    forge::ui::Widget root("root");
    FORGE_CHECK_EQ(root.child_count(), static_cast<std::size_t>(0));

    forge::ui::Widget* child = root.add_child(std::make_unique<forge::ui::Widget>("child"));
    FORGE_REQUIRE(child != nullptr);
    FORGE_CHECK_EQ(root.child_count(), static_cast<std::size_t>(1));
    FORGE_CHECK(root.find_child("child") == child);

    FORGE_CHECK(root.remove_child("child"));
    FORGE_CHECK_EQ(root.child_count(), static_cast<std::size_t>(0));
    FORGE_CHECK(!root.remove_child("child"));
}

FORGE_TEST_CASE("ui.widget_flags")
{
    forge::ui::Widget widget("w");
    FORGE_CHECK(widget.visible());
    FORGE_CHECK(widget.enabled());
    widget.set_visible(false);
    widget.set_enabled(false);
    FORGE_CHECK(!widget.visible());
    FORGE_CHECK(!widget.enabled());
    widget.set_bounds(forge::ui::Rect{1, 2, 3, 4});
    FORGE_CHECK_NEAR(widget.bounds().width, 3.0f, 1e-4f);
}

FORGE_TEST_CASE("ui.panel_collapse")
{
    forge::ui::Panel panel("p", "Properties");
    FORGE_CHECK_EQ(panel.title(), std::string("Properties"));
    FORGE_CHECK(!panel.collapsed());
    panel.toggle_collapsed();
    FORGE_CHECK(panel.collapsed());
    panel.set_collapsed(false);
    FORGE_CHECK(!panel.collapsed());
}

FORGE_TEST_CASE("ui.menu_stores_command_ids")
{
    forge::ui::Menu menu("File");
    menu.add_item("New", "file.new");
    menu.add_separator();
    menu.add_item("Save", "file.save");
    FORGE_CHECK_EQ(menu.size(), static_cast<std::size_t>(3));
    FORGE_CHECK_EQ(menu.index_of_command("file.save"), 2);
    FORGE_CHECK_EQ(menu.index_of_command("missing"), -1);
    FORGE_CHECK(menu.items()[1].separator);
}

FORGE_TEST_CASE("ui.toolbar_active_tool")
{
    forge::ui::Toolbar toolbar("transform");
    toolbar.add_tool("transform.move", "Move");
    toolbar.add_tool("transform.rotate", "Rotate");
    FORGE_CHECK(toolbar.active_tool_id().empty());
    FORGE_CHECK(toolbar.set_active_tool("transform.rotate"));
    FORGE_CHECK_EQ(toolbar.active_tool_id(), std::string("transform.rotate"));
    FORGE_CHECK(!toolbar.set_active_tool("missing"));
}

FORGE_TEST_CASE("ui.status_bar_messages_and_progress")
{
    forge::ui::StatusBar status;
    status.set_message("Ready");
    FORGE_CHECK_EQ(status.message(), std::string("Ready"));
    FORGE_CHECK(!status.has_progress());

    status.set_progress(0.5f);
    FORGE_CHECK(status.has_progress());
    FORGE_CHECK_NEAR(status.progress(), 0.5f, 1e-4f);
    status.set_progress(5.0f); // clamped
    FORGE_CHECK_NEAR(status.progress(), 1.0f, 1e-4f);

    status.push_message("Working");
    FORGE_CHECK_EQ(status.history().size(), static_cast<std::size_t>(1));
    status.clear_progress();
    FORGE_CHECK(!status.has_progress());
}
