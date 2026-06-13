#include "test_framework.hpp"

#include "forge/ui/docking.hpp"
#include "forge/ui/screen.hpp"

using forge::ui::EditorType;
using forge::ui::Screen;
using forge::ui::SplitDirection;

FORGE_TEST_CASE("ui.screen_split_vertical_creates_two_areas")
{
    Screen screen(EditorType::Viewport3D);
    FORGE_CHECK_EQ(screen.area_count(), static_cast<std::size_t>(1));

    const forge::ui::AreaId original = screen.active_area_id();
    const forge::ui::AreaId created =
        screen.split_area(original, SplitDirection::Vertical, 0.5f, EditorType::Outliner);

    FORGE_CHECK(created != forge::ui::kInvalidAreaId);
    FORGE_CHECK_EQ(screen.area_count(), static_cast<std::size_t>(2));
    FORGE_REQUIRE(screen.find_area(created) != nullptr);
    FORGE_CHECK(screen.find_area(created)->editor_type() == EditorType::Outliner);
    FORGE_CHECK(screen.find_area(original)->editor_type() == EditorType::Viewport3D);
}

FORGE_TEST_CASE("ui.screen_split_horizontal_creates_two_areas")
{
    Screen screen(EditorType::Viewport3D);
    const forge::ui::AreaId original = screen.active_area_id();
    const forge::ui::AreaId created =
        screen.split_area(original, SplitDirection::Horizontal, 0.5f);
    FORGE_CHECK(created != forge::ui::kInvalidAreaId);
    FORGE_CHECK_EQ(screen.area_count(), static_cast<std::size_t>(2));
    // No editor specified -> copies the original.
    FORGE_CHECK(screen.find_area(created)->editor_type() == EditorType::Viewport3D);
}

FORGE_TEST_CASE("ui.screen_split_unknown_area_fails")
{
    Screen screen(EditorType::Viewport3D);
    FORGE_CHECK(screen.split_area(9999, SplitDirection::Vertical) ==
                forge::ui::kInvalidAreaId);
}

FORGE_TEST_CASE("ui.screen_close_area_merges_sibling")
{
    Screen screen(EditorType::Viewport3D);
    const forge::ui::AreaId original = screen.active_area_id();
    const forge::ui::AreaId created =
        screen.split_area(original, SplitDirection::Vertical, 0.5f, EditorType::Properties);
    FORGE_CHECK_EQ(screen.area_count(), static_cast<std::size_t>(2));

    FORGE_CHECK(screen.close_area(created));
    FORGE_CHECK_EQ(screen.area_count(), static_cast<std::size_t>(1));
    FORGE_REQUIRE(screen.find_area(original) != nullptr);
    FORGE_CHECK(screen.find_area(created) == nullptr);
}

FORGE_TEST_CASE("ui.screen_cannot_close_last_area")
{
    Screen screen(EditorType::Viewport3D);
    const forge::ui::AreaId only = screen.active_area_id();
    FORGE_CHECK(!screen.close_area(only));
    FORGE_CHECK_EQ(screen.area_count(), static_cast<std::size_t>(1));
}

FORGE_TEST_CASE("ui.screen_close_active_updates_active")
{
    Screen screen(EditorType::Viewport3D);
    const forge::ui::AreaId original = screen.active_area_id();
    const forge::ui::AreaId created =
        screen.split_area(original, SplitDirection::Vertical, 0.5f);
    FORGE_CHECK(screen.set_active_area(created));
    FORGE_CHECK_EQ(screen.active_area_id(), created);

    FORGE_CHECK(screen.close_area(created));
    // Active area must still be valid after closing the previously-active one.
    FORGE_REQUIRE(screen.active_area() != nullptr);
    FORGE_CHECK_EQ(screen.active_area_id(), original);
}

FORGE_TEST_CASE("ui.screen_nested_splits")
{
    Screen screen(EditorType::Viewport3D);
    const forge::ui::AreaId a = screen.active_area_id();
    const forge::ui::AreaId b = screen.split_area(a, SplitDirection::Vertical, 0.5f);
    const forge::ui::AreaId c = screen.split_area(b, SplitDirection::Horizontal, 0.5f);
    const forge::ui::AreaId d = screen.split_area(a, SplitDirection::Horizontal, 0.5f);
    FORGE_CHECK_EQ(screen.area_count(), static_cast<std::size_t>(4));
    FORGE_CHECK(screen.find_area(c) != nullptr);
    FORGE_CHECK(screen.find_area(d) != nullptr);

    // Closing reduces the count and keeps the tree consistent.
    FORGE_CHECK(screen.close_area(c));
    FORGE_CHECK_EQ(screen.area_count(), static_cast<std::size_t>(3));
}
