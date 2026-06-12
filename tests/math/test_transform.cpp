#include "forge/math/angle.hpp"
#include "forge/math/transform.hpp"
#include "test_framework.hpp"

using forge::Mat4;
using forge::Quat;
using forge::Transform;
using forge::Vec3;

FORGE_TEST_CASE("math.transform_identity")
{
    const Transform identity = Transform::identity();
    const Vec3 v{1.0f, 2.0f, 3.0f};
    FORGE_CHECK(near_equal(identity.transform_point(v), v));
    FORGE_CHECK(near_equal(identity.to_matrix(), Mat4::identity()));
}

FORGE_TEST_CASE("math.transform_to_matrix_matches_components")
{
    const Transform transform{
        Vec3{5.0f, -2.0f, 1.0f},
        Quat::from_axis_angle(Vec3::unit_z(), forge::to_radians(90.0f)),
        Vec3{2.0f, 2.0f, 2.0f},
    };

    // Applying the matrix and applying the components must agree.
    const Vec3 v{1.0f, 0.5f, -0.25f};
    const Vec3 by_components = transform.transform_point(v);
    const Vec3 by_matrix = forge::transform_point(transform.to_matrix(), v);
    FORGE_CHECK(near_equal(by_components, by_matrix, 1e-4f));

    // Spot check: scale 2 then rotate 90 deg about Z then translate.
    // (1,0,0) -> (2,0,0) -> (0,2,0) -> (5,0,1).
    const Vec3 result = transform.transform_point(Vec3::unit_x());
    FORGE_CHECK(near_equal(result, Vec3{5.0f, 0.0f, 1.0f}, 1e-4f));
}

FORGE_TEST_CASE("math.transform_direction_ignores_translation_and_scale")
{
    const Transform transform{
        Vec3{100.0f, 100.0f, 100.0f},
        Quat::from_axis_angle(Vec3::unit_z(), forge::to_radians(90.0f)),
        Vec3{7.0f, 7.0f, 7.0f},
    };
    const Vec3 direction = transform.transform_direction(Vec3::unit_x());
    FORGE_CHECK(near_equal(direction, Vec3::unit_y(), 1e-5f));
}

FORGE_TEST_CASE("math.transform_inverse_round_trip")
{
    const Transform transform{
        Vec3{3.0f, -1.0f, 2.0f},
        Quat::from_axis_angle(Vec3{0.0f, 1.0f, 1.0f}, 0.9f),
        Vec3{2.0f, 2.0f, 2.0f}, // uniform scale: inverse is exact
    };
    const Transform inverse = transform.inverse();

    const Vec3 v{1.5f, 0.25f, -3.0f};
    FORGE_CHECK(near_equal(inverse.transform_point(transform.transform_point(v)), v, 1e-3f));
}

FORGE_TEST_CASE("math.transform_combine")
{
    const Transform parent{
        Vec3{10.0f, 0.0f, 0.0f},
        Quat::from_axis_angle(Vec3::unit_z(), forge::to_radians(90.0f)),
        Vec3::one(),
    };
    const Transform child{
        Vec3{1.0f, 0.0f, 0.0f},
        Quat::identity(),
        Vec3::one(),
    };

    const Transform combined = forge::combine(parent, child);

    // Combined application equals sequential application.
    const Vec3 v{0.5f, 0.5f, 0.0f};
    const Vec3 sequential = parent.transform_point(child.transform_point(v));
    FORGE_CHECK(near_equal(combined.transform_point(v), sequential, 1e-4f));

    // The child origin lands at the parent's transform of (1,0,0): (10,1,0).
    FORGE_CHECK(near_equal(combined.transform_point(Vec3::zero()),
                           Vec3{10.0f, 1.0f, 0.0f}, 1e-4f));
}
