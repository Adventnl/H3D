#include "forge/math/vec2.hpp"
#include "test_framework.hpp"

using forge::Vec2;

FORGE_TEST_CASE("math.vec2_arithmetic")
{
    const Vec2 a{1.0f, 2.0f};
    const Vec2 b{3.0f, -1.0f};

    FORGE_CHECK(near_equal(a + b, Vec2{4.0f, 1.0f}));
    FORGE_CHECK(near_equal(a - b, Vec2{-2.0f, 3.0f}));
    FORGE_CHECK(near_equal(a * 2.0f, Vec2{2.0f, 4.0f}));
    FORGE_CHECK(near_equal(2.0f * a, Vec2{2.0f, 4.0f}));
    FORGE_CHECK(near_equal(b / 2.0f, Vec2{1.5f, -0.5f}));
    FORGE_CHECK(near_equal(-a, Vec2{-1.0f, -2.0f}));
}

FORGE_TEST_CASE("math.vec2_dot_and_length")
{
    const Vec2 a{3.0f, 4.0f};
    FORGE_CHECK_NEAR(dot(a, a), 25.0f, 1e-6f);
    FORGE_CHECK_NEAR(a.length(), 5.0f, 1e-6f);
    FORGE_CHECK_NEAR(a.length_squared(), 25.0f, 1e-6f);
    FORGE_CHECK_NEAR(dot(Vec2{1.0f, 0.0f}, Vec2{0.0f, 1.0f}), 0.0f, 1e-6f);
}

FORGE_TEST_CASE("math.vec2_normalization")
{
    const Vec2 v{3.0f, 4.0f};
    const Vec2 unit = v.normalized();
    FORGE_CHECK_NEAR(unit.length(), 1.0f, 1e-6f);
    FORGE_CHECK(near_equal(unit, Vec2{0.6f, 0.8f}));

    // Safe normalization of a zero vector returns the fallback.
    const Vec2 fallback = Vec2::zero().safe_normalized(Vec2{1.0f, 0.0f});
    FORGE_CHECK(near_equal(fallback, Vec2{1.0f, 0.0f}));
}

FORGE_TEST_CASE("math.vec2_distance_and_lerp")
{
    FORGE_CHECK_NEAR(distance(Vec2{0.0f, 0.0f}, Vec2{3.0f, 4.0f}), 5.0f, 1e-6f);

    const Vec2 mid = lerp(Vec2{0.0f, 0.0f}, Vec2{10.0f, -2.0f}, 0.5f);
    FORGE_CHECK(near_equal(mid, Vec2{5.0f, -1.0f}));
}

FORGE_TEST_CASE("math.vec2_constants")
{
    FORGE_CHECK(near_equal(Vec2::zero(), Vec2{0.0f, 0.0f}));
    FORGE_CHECK(near_equal(Vec2::one(), Vec2{1.0f, 1.0f}));
    FORGE_CHECK(near_equal(Vec2(2.5f), Vec2{2.5f, 2.5f}));
}
