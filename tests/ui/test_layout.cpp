#include "test_framework.hpp"

#include "forge/ui/layout.hpp"
#include "forge/ui/rect.hpp"

using forge::ui::Rect;

FORGE_TEST_CASE("ui.rect_contains")
{
    const Rect rect{10.0f, 10.0f, 100.0f, 50.0f};
    FORGE_CHECK(rect.contains(10.0f, 10.0f));
    FORGE_CHECK(rect.contains(50.0f, 30.0f));
    FORGE_CHECK(!rect.contains(110.0f, 30.0f)); // on right edge is exclusive
    FORGE_CHECK(!rect.contains(5.0f, 5.0f));
    FORGE_CHECK_NEAR(rect.right(), 110.0f, 1e-4f);
    FORGE_CHECK_NEAR(rect.bottom(), 60.0f, 1e-4f);
}

FORGE_TEST_CASE("ui.rect_intersects")
{
    const Rect a{0.0f, 0.0f, 100.0f, 100.0f};
    const Rect b{50.0f, 50.0f, 100.0f, 100.0f};
    const Rect c{200.0f, 200.0f, 10.0f, 10.0f};
    FORGE_CHECK(a.intersects(b));
    FORGE_CHECK(!a.intersects(c));
}

FORGE_TEST_CASE("ui.rect_split_and_inset")
{
    const Rect rect{0.0f, 0.0f, 100.0f, 80.0f};

    const Rect left = rect.split_left(30.0f);
    FORGE_CHECK_NEAR(left.width, 30.0f, 1e-4f);
    FORGE_CHECK_NEAR(left.x, 0.0f, 1e-4f);

    const Rect right = rect.split_right(30.0f);
    FORGE_CHECK_NEAR(right.x, 70.0f, 1e-4f);
    FORGE_CHECK_NEAR(right.width, 30.0f, 1e-4f);

    const Rect top = rect.split_top(20.0f);
    FORGE_CHECK_NEAR(top.height, 20.0f, 1e-4f);

    const Rect inset = rect.inset(10.0f);
    FORGE_CHECK_NEAR(inset.x, 10.0f, 1e-4f);
    FORGE_CHECK_NEAR(inset.width, 80.0f, 1e-4f);
    FORGE_CHECK_NEAR(inset.height, 60.0f, 1e-4f);
}

FORGE_TEST_CASE("ui.layout_split_helpers")
{
    const Rect rect{0.0f, 0.0f, 100.0f, 100.0f};
    const auto [l, r] = forge::ui::split_horizontal(rect, 0.25f);
    FORGE_CHECK_NEAR(l.width, 25.0f, 1e-4f);
    FORGE_CHECK_NEAR(r.width, 75.0f, 1e-4f);
    FORGE_CHECK_NEAR(r.x, 25.0f, 1e-4f);

    const auto [t, b] = forge::ui::split_vertical(rect, 0.5f);
    FORGE_CHECK_NEAR(t.height, 50.0f, 1e-4f);
    FORGE_CHECK_NEAR(b.y, 50.0f, 1e-4f);
}

FORGE_TEST_CASE("ui.layout_columns_rows")
{
    const Rect rect{0.0f, 0.0f, 90.0f, 60.0f};
    const auto cols = forge::ui::layout_columns(rect, 3);
    FORGE_REQUIRE(cols.size() == static_cast<std::size_t>(3));
    FORGE_CHECK_NEAR(cols[0].width, 30.0f, 1e-4f);
    FORGE_CHECK_NEAR(cols[2].x, 60.0f, 1e-4f);

    const auto rows = forge::ui::layout_rows(rect, 2);
    FORGE_REQUIRE(rows.size() == static_cast<std::size_t>(2));
    FORGE_CHECK_NEAR(rows[0].height, 30.0f, 1e-4f);

    // Zero count is safe.
    FORGE_CHECK(forge::ui::layout_columns(rect, 0).empty());
}

FORGE_TEST_CASE("ui.layout_weighted_columns")
{
    const Rect rect{0.0f, 0.0f, 100.0f, 10.0f};
    const auto cols = forge::ui::layout_weighted_columns(rect, {1.0f, 3.0f});
    FORGE_REQUIRE(cols.size() == static_cast<std::size_t>(2));
    FORGE_CHECK_NEAR(cols[0].width, 25.0f, 1e-3f);
    FORGE_CHECK_NEAR(cols[1].width, 75.0f, 1e-3f);
}
