#include <cstdint>

#include "bench_framework.hpp"
#include "forge/math/mat4.hpp"
#include "forge/math/quat.hpp"
#include "forge/math/vec3.hpp"

using forge::Mat4;
using forge::Quat;
using forge::Vec3;
using forge::bench::do_not_optimize;

FORGE_BENCHMARK("math.vec3_normalize")
{
    Vec3 value{1.0f, 2.0f, 3.0f};
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        // Vary the input so the compiler cannot hoist the computation.
        value.x = static_cast<float>((index % 31) + 1);
        do_not_optimize(value.normalized());
    }
}

FORGE_BENCHMARK("math.vec3_dot_cross")
{
    Vec3 a{1.0f, 2.0f, 3.0f};
    const Vec3 b{-2.0f, 0.5f, 4.0f};
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        a.y = static_cast<float>(index % 17);
        do_not_optimize(dot(a, b));
        do_not_optimize(cross(a, b));
    }
}

FORGE_BENCHMARK("math.mat4_multiply")
{
    Mat4 a = Mat4::translation(Vec3{1.0f, 2.0f, 3.0f});
    const Mat4 b = Mat4::scale(Vec3{1.5f, 0.5f, 2.0f});
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        a.columns[3].x = static_cast<float>(index % 13);
        do_not_optimize(a * b);
    }
}

FORGE_BENCHMARK("math.quat_rotate_vector")
{
    const Quat rotation = Quat::from_axis_angle(Vec3::unit_z(), 0.5f);
    Vec3 value{1.0f, 0.0f, 0.0f};
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        value.z = static_cast<float>(index % 7);
        do_not_optimize(rotation.rotate_vector(value));
    }
}
