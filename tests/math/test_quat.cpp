#include "forge/math/angle.hpp"
#include "forge/math/quat.hpp"
#include "test_framework.hpp"

using forge::Quat;
using forge::Vec3;

FORGE_TEST_CASE("math.quat_identity")
{
    const Quat identity = Quat::identity();
    const Vec3 v{1.0f, 2.0f, 3.0f};
    FORGE_CHECK(near_equal(identity.rotate_vector(v), v));
    FORGE_CHECK_NEAR(identity.length(), 1.0f, 1e-6f);
}

FORGE_TEST_CASE("math.quat_axis_angle_rotation")
{
    // 90 degrees around Z: +X -> +Y.
    const Quat rotation_z = Quat::from_axis_angle(Vec3::unit_z(), forge::to_radians(90.0f));
    FORGE_CHECK(near_equal(rotation_z.rotate_vector(Vec3::unit_x()), Vec3::unit_y(), 1e-5f));

    // 90 degrees around X: +Y -> +Z.
    const Quat rotation_x = Quat::from_axis_angle(Vec3::unit_x(), forge::to_radians(90.0f));
    FORGE_CHECK(near_equal(rotation_x.rotate_vector(Vec3::unit_y()), Vec3::unit_z(), 1e-5f));

    // 180 degrees around Y: +X -> -X.
    const Quat rotation_y = Quat::from_axis_angle(Vec3::unit_y(), forge::to_radians(180.0f));
    FORGE_CHECK(near_equal(rotation_y.rotate_vector(Vec3::unit_x()), -Vec3::unit_x(), 1e-5f));

    // A zero axis yields identity rather than NaN.
    const Quat degenerate = Quat::from_axis_angle(Vec3::zero(), 1.0f);
    FORGE_CHECK(near_equal(degenerate, Quat::identity()));
}

FORGE_TEST_CASE("math.quat_multiplication_composes")
{
    // Two 45-degree rotations equal one 90-degree rotation.
    const Quat half = Quat::from_axis_angle(Vec3::unit_z(), forge::to_radians(45.0f));
    const Quat full = Quat::from_axis_angle(Vec3::unit_z(), forge::to_radians(90.0f));
    const Vec3 v{1.0f, 0.0f, 0.0f};
    FORGE_CHECK(near_equal((half * half).rotate_vector(v), full.rotate_vector(v), 1e-5f));
}

FORGE_TEST_CASE("math.quat_conjugate_and_inverse")
{
    const Quat rotation = Quat::from_axis_angle(Vec3{1.0f, 1.0f, 0.0f}, 0.7f);
    const Vec3 v{2.0f, -1.0f, 0.5f};

    // inverse(rotate(v)) round-trips.
    const Vec3 rotated = rotation.rotate_vector(v);
    FORGE_CHECK(near_equal(rotation.inverse().rotate_vector(rotated), v, 1e-4f));

    // For unit quaternions the conjugate equals the inverse.
    FORGE_CHECK(near_equal(rotation.conjugate(), rotation.inverse(), 1e-5f));
}

FORGE_TEST_CASE("math.quat_normalized")
{
    const Quat scaled{0.0f, 0.0f, 2.0f, 2.0f};
    const Quat unit = scaled.normalized();
    FORGE_CHECK_NEAR(unit.length(), 1.0f, 1e-6f);
}

FORGE_TEST_CASE("math.quat_slerp")
{
    const Quat start = Quat::identity();
    const Quat end = Quat::from_axis_angle(Vec3::unit_z(), forge::to_radians(90.0f));

    FORGE_CHECK(near_equal(slerp(start, end, 0.0f), start, 1e-5f));
    FORGE_CHECK(near_equal(slerp(start, end, 1.0f), end, 1e-5f));

    // Halfway should be a 45-degree rotation.
    const Quat halfway = slerp(start, end, 0.5f);
    const Quat expected = Quat::from_axis_angle(Vec3::unit_z(), forge::to_radians(45.0f));
    FORGE_CHECK(near_equal(halfway, expected, 1e-4f));
}
