#include "test_framework.hpp"

#include "forge/ui/screen.hpp"

using forge::ui::EditorType;
using forge::ui::Rect;
using forge::ui::RegionType;
using forge::ui::Screen;
using forge::ui::SplitDirection;

FORGE_TEST_CASE("ui.screen_default_has_one_active_area")
{
    Screen screen(EditorType::Viewport3D);
    FORGE_CHECK_EQ(screen.area_count(), static_cast<std::size_t>(1));
    FORGE_REQUIRE(screen.active_area() != nullptr);
    FORGE_CHECK(screen.active_area()->editor_type() == EditorType::Viewport3D);
    FORGE_CHECK(screen.active_area()->active());
}

FORGE_TEST_CASE("ui.screen_area_has_default_regions")
{
    Screen screen(EditorType::Outliner);
    const forge::ui::Area* area = screen.active_area();
    FORGE_REQUIRE(area != nullptr);
    FORGE_CHECK(area->find_region(RegionType::Header) != nullptr);
    FORGE_CHECK(area->find_region(RegionType::Main) != nullptr);
}

FORGE_TEST_CASE("ui.screen_change_editor_type")
{
    Screen screen(EditorType::Viewport3D);
    const forge::ui::AreaId id = screen.active_area_id();
    FORGE_CHECK(screen.set_area_editor_type(id, EditorType::Properties));
    FORGE_CHECK(screen.find_area(id)->editor_type() == EditorType::Properties);
    FORGE_CHECK(!screen.set_area_editor_type(9999, EditorType::Properties));
}

FORGE_TEST_CASE("ui.screen_compute_layout_tiles_areas")
{
    Screen screen(EditorType::Viewport3D);
    const forge::ui::AreaId a = screen.active_area_id();
    const forge::ui::AreaId b = screen.split_area(a, SplitDirection::Vertical, 0.25f);

    const Rect bounds{0.0f, 0.0f, 1000.0f, 800.0f};
    screen.compute_layout(bounds);

    const forge::ui::Area* area_a = screen.find_area(a);
    const forge::ui::Area* area_b = screen.find_area(b);
    FORGE_REQUIRE(area_a != nullptr);
    FORGE_REQUIRE(area_b != nullptr);

    // Vertical split at 0.25: left 250 wide, right 750 wide, full height.
    FORGE_CHECK_NEAR(area_a->rect().width, 250.0f, 1e-2f);
    FORGE_CHECK_NEAR(area_b->rect().width, 750.0f, 1e-2f);
    FORGE_CHECK_NEAR(area_a->rect().height, 800.0f, 1e-2f);
    FORGE_CHECK_NEAR(area_b->rect().x, 250.0f, 1e-2f);

    // Regions tile within the area: header on top, main below it.
    const forge::ui::Region* header = area_a->find_region(RegionType::Header);
    const forge::ui::Region* main = area_a->find_region(RegionType::Main);
    FORGE_REQUIRE(header != nullptr);
    FORGE_REQUIRE(main != nullptr);
    FORGE_CHECK_NEAR(header->rect().y, 0.0f, 1e-2f);
    FORGE_CHECK(main->rect().y >= header->rect().bottom() - 1e-2f);
}

FORGE_TEST_CASE("ui.screen_areas_cover_bounds_without_overlap")
{
    Screen screen(EditorType::Viewport3D);
    const forge::ui::AreaId a = screen.active_area_id();
    screen.split_area(a, SplitDirection::Horizontal, 0.5f);
    const Rect bounds{0.0f, 0.0f, 400.0f, 300.0f};
    screen.compute_layout(bounds);

    float total = 0.0f;
    for (const forge::ui::Area* area : screen.areas())
    {
        total += area->rect().area();
    }
    FORGE_CHECK_NEAR(total, bounds.area(), 1.0f);
}
