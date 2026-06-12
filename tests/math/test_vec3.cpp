#include "forge/math/vec3.hpp"
#include "test_framework.hpp"

using forge::Vec3;

FORGE_TEST_CASE("math.vec3_arithmetic")
{
    const Vec3 a{1.0f, 2.0f, 3.0f};
    const Vec3 b{4.0f, -2.0f, 0.5f};

    FORGE_CHECK(near_equal(a + b, Vec3{5.0f, 0.0f, 3.5f}));
    FORGE_CHECK(near_equal(a - b, Vec3{-3.0f, 4.0f, 2.5f}));
    FORGE_CHECK(near_equal(a * 2.0f, Vec3{2.0f, 4.0f, 6.0f}));
    FORGE_CHECK(near_equal(a / 2.0f, Vec3{0.5f, 1.0f, 1.5f}));
    FORGE_CHECK(near_equal(a * b, Vec3{4.0f, -4.0f, 1.5f})); // component-wise
}

FORGE_TEST_CASE("math.vec3_dot")
{
    const Vec3 a{1.0f, 2.0f, 3.0f};
    const Vec3 b{4.0f, 5.0f, 6.0f};
    FORGE_CHECK_NEAR(dot(a, b), 32.0f, 1e-6f);
    FORGE_CHECK_NEAR(dot(Vec3::unit_x(), Vec3::unit_y()), 0.0f, 1e-6f);
}

FORGE_TEST_CASE("math.vec3_cross")
{
    FORGE_CHECK(near_equal(cross(Vec3::unit_x(), Vec3::unit_y()), Vec3::unit_z()));
    FORGE_CHECK(near_equal(cross(Vec3::unit_y(), Vec3::unit_z()), Vec3::unit_x()));
    FORGE_CHECK(near_equal(cross(Vec3::unit_z(), Vec3::unit_x()), Vec3::unit_y()));

    // Anti-commutativity.
    const Vec3 a{1.0f, 2.0f, 3.0f};
    const Vec3 b{-2.0f, 0.5f, 4.0f};
    FORGE_CHECK(near_equal(cross(a, b), -cross(b, a)));

    // The cross product is orthogonal to both inputs.
    FORGE_CHECK_NEAR(dot(cross(a, b), a), 0.0f, 1e-4f);
    FORGE_CHECK_NEAR(dot(cross(a, b), b), 0.0f, 1e-4f);
}

FORGE_TEST_CASE("math.vec3_length_and_normalization")
{
    const Vec3 v{2.0f, 3.0f, 6.0f};
    FORGE_CHECK_NEAR(v.length(), 7.0f, 1e-6f);
    FORGE_CHECK_NEAR(v.length_squared(), 49.0f, 1e-6f);
    FORGE_CHECK_NEAR(v.normalized().length(), 1.0f, 1e-6f);

    // Safe normalization of zero returns the fallback (default: zero).
    FORGE_CHECK(near_equal(Vec3::zero().safe_normalized(), Vec3::zero()));
    FORGE_CHECK(near_equal(Vec3::zero().safe_normalized(Vec3::unit_y()), Vec3::unit_y()));
}

FORGE_TEST_CASE("math.vec3_min_max_clamp")
{
    const Vec3 a{1.0f, 5.0f, -2.0f};
    const Vec3 b{3.0f, 2.0f, 0.0f};

    FORGE_CHECK(near_equal(forge::min(a, b), Vec3{1.0f, 2.0f, -2.0f}));
    FORGE_CHECK(near_equal(forge::max(a, b), Vec3{3.0f, 5.0f, 0.0f}));

    const Vec3 clamped = forge::clamp(Vec3{-5.0f, 0.5f, 9.0f}, Vec3::zero(), Vec3::one());
    FORGE_CHECK(near_equal(clamped, Vec3{0.0f, 0.5f, 1.0f}));
}

FORGE_TEST_CASE("math.vec3_lerp_and_distance")
{
    const Vec3 start{0.0f, 0.0f, 0.0f};
    const Vec3 end{10.0f, 20.0f, 30.0f};
    FORGE_CHECK(near_equal(lerp(start, end, 0.0f), start));
    FORGE_CHECK(near_equal(lerp(start, end, 1.0f), end));
    FORGE_CHECK(near_equal(lerp(start, end, 0.5f), Vec3{5.0f, 10.0f, 15.0f}));

    FORGE_CHECK_NEAR(distance(Vec3{1.0f, 1.0f, 1.0f}, Vec3{1.0f, 1.0f, 1.0f}), 0.0f, 1e-6f);
    FORGE_CHECK_NEAR(distance(Vec3::zero(), Vec3{2.0f, 3.0f, 6.0f}), 7.0f, 1e-6f);
}
