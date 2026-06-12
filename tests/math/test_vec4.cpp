#include "forge/math/vec4.hpp"
#include "test_framework.hpp"

using forge::Vec3;
using forge::Vec4;

FORGE_TEST_CASE("math.vec4_arithmetic")
{
    const Vec4 a{1.0f, 2.0f, 3.0f, 4.0f};
    const Vec4 b{0.5f, -1.0f, 2.0f, 0.0f};

    FORGE_CHECK(near_equal(a + b, Vec4{1.5f, 1.0f, 5.0f, 4.0f}));
    FORGE_CHECK(near_equal(a - b, Vec4{0.5f, 3.0f, 1.0f, 4.0f}));
    FORGE_CHECK(near_equal(a * 2.0f, Vec4{2.0f, 4.0f, 6.0f, 8.0f}));
    FORGE_CHECK(near_equal(a / 2.0f, Vec4{0.5f, 1.0f, 1.5f, 2.0f}));
}

FORGE_TEST_CASE("math.vec4_dot")
{
    const Vec4 a{1.0f, 2.0f, 3.0f, 4.0f};
    const Vec4 b{5.0f, 6.0f, 7.0f, 8.0f};
    FORGE_CHECK_NEAR(dot(a, b), 70.0f, 1e-6f);
}

FORGE_TEST_CASE("math.vec4_vec3_conversion")
{
    const Vec3 v{1.0f, 2.0f, 3.0f};
    const Vec4 point{v, 1.0f};
    const Vec4 direction{v, 0.0f};

    FORGE_CHECK_NEAR(point.w, 1.0f, 1e-6f);
    FORGE_CHECK_NEAR(direction.w, 0.0f, 1e-6f);
    FORGE_CHECK(near_equal(point.xyz(), v));
}
