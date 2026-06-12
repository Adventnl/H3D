#include "forge/math/angle.hpp"
#include "forge/math/mat4.hpp"
#include "forge/math/quat.hpp"
#include "test_framework.hpp"

using forge::Mat4;
using forge::Quat;
using forge::Vec3;
using forge::Vec4;

FORGE_TEST_CASE("math.mat4_identity")
{
    const Mat4 identity = Mat4::identity();
    const Vec4 v{1.0f, 2.0f, 3.0f, 1.0f};
    FORGE_CHECK(near_equal(identity * v, v));
    FORGE_CHECK(near_equal(Mat4{}, identity)); // default construction is identity
}

FORGE_TEST_CASE("math.mat4_translation")
{
    const Mat4 translation = Mat4::translation(Vec3{10.0f, -5.0f, 2.0f});
    const Vec3 moved = forge::transform_point(translation, Vec3{1.0f, 1.0f, 1.0f});
    FORGE_CHECK(near_equal(moved, Vec3{11.0f, -4.0f, 3.0f}));

    // Directions are unaffected by translation.
    const Vec3 direction = forge::transform_direction(translation, Vec3::unit_x());
    FORGE_CHECK(near_equal(direction, Vec3::unit_x()));
}

FORGE_TEST_CASE("math.mat4_scale")
{
    const Mat4 scale = Mat4::scale(Vec3{2.0f, 3.0f, 4.0f});
    const Vec3 scaled = forge::transform_point(scale, Vec3{1.0f, 1.0f, 1.0f});
    FORGE_CHECK(near_equal(scaled, Vec3{2.0f, 3.0f, 4.0f}));
}

FORGE_TEST_CASE("math.mat4_multiply")
{
    // Translate then scale vs. scale then translate differ; verify order.
    const Mat4 translate = Mat4::translation(Vec3{1.0f, 0.0f, 0.0f});
    const Mat4 scale = Mat4::scale(Vec3{2.0f, 2.0f, 2.0f});

    // (translate * scale): scale first, then translate.
    const Vec3 a = forge::transform_point(translate * scale, Vec3{1.0f, 0.0f, 0.0f});
    FORGE_CHECK(near_equal(a, Vec3{3.0f, 0.0f, 0.0f}));

    // (scale * translate): translate first, then scale.
    const Vec3 b = forge::transform_point(scale * translate, Vec3{1.0f, 0.0f, 0.0f});
    FORGE_CHECK(near_equal(b, Vec3{4.0f, 0.0f, 0.0f}));

    FORGE_CHECK(near_equal(translate * Mat4::identity(), translate));
}

FORGE_TEST_CASE("math.mat4_from_quat")
{
    // 90 degrees around Z maps +X to +Y.
    const Quat rotation = Quat::from_axis_angle(Vec3::unit_z(), forge::to_radians(90.0f));
    const Mat4 matrix = Mat4::from_quat(rotation);
    const Vec3 rotated = forge::transform_point(matrix, Vec3::unit_x());
    FORGE_CHECK(near_equal(rotated, Vec3::unit_y(), 1e-5f));

    // The matrix path agrees with direct quaternion rotation.
    const Vec3 arbitrary{0.3f, -1.2f, 2.5f};
    FORGE_CHECK(near_equal(forge::transform_point(matrix, arbitrary),
                           rotation.rotate_vector(arbitrary), 1e-4f));
}

FORGE_TEST_CASE("math.mat4_transpose")
{
    const Mat4 translation = Mat4::translation(Vec3{1.0f, 2.0f, 3.0f});
    const Mat4 transposed = translation.transposed();
    FORGE_CHECK_NEAR(transposed.at(3, 0), 1.0f, 1e-6f);
    FORGE_CHECK_NEAR(transposed.at(3, 1), 2.0f, 1e-6f);
    FORGE_CHECK(near_equal(transposed.transposed(), translation));
}

FORGE_TEST_CASE("math.mat4_projections")
{
    // A point on the near plane straight ahead maps to NDC z = -1.
    const Mat4 perspective =
        Mat4::perspective(forge::to_radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    const Vec4 near_point = perspective * Vec4{0.0f, 0.0f, -0.1f, 1.0f};
    FORGE_CHECK_NEAR(near_point.z / near_point.w, -1.0f, 1e-4f);

    // Orthographic maps the box corners to [-1, 1].
    const Mat4 ortho = Mat4::orthographic(-2.0f, 2.0f, -1.0f, 1.0f, 0.0f, 10.0f);
    const Vec4 corner = ortho * Vec4{2.0f, 1.0f, -10.0f, 1.0f};
    FORGE_CHECK(near_equal(corner, Vec4{1.0f, 1.0f, 1.0f, 1.0f}, 1e-5f));
}

FORGE_TEST_CASE("math.mat4_look_at")
{
    // Looking from +Z toward the origin: the eye-space forward is -Z and the
    // target lands on the negative Z axis.
    const Mat4 view = Mat4::look_at(Vec3{0.0f, 0.0f, 5.0f}, Vec3::zero(), Vec3::unit_y());
    const Vec3 target_in_view = forge::transform_point(view, Vec3::zero());
    FORGE_CHECK(near_equal(target_in_view, Vec3{0.0f, 0.0f, -5.0f}, 1e-5f));

    // The eye itself maps to the view-space origin.
    const Vec3 eye_in_view = forge::transform_point(view, Vec3{0.0f, 0.0f, 5.0f});
    FORGE_CHECK(near_equal(eye_in_view, Vec3::zero(), 1e-5f));
}
