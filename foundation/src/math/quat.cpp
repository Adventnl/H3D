#include "forge/math/quat.hpp"

#include <cmath>

#include "forge/foundation/assert.hpp"

namespace forge
{

Quat Quat::from_axis_angle(const Vec3& axis, float angle_radians)
{
    const Vec3 unit_axis = axis.safe_normalized();
    if (unit_axis == Vec3::zero())
    {
        return identity();
    }

    const float half_angle = angle_radians * 0.5f;
    const float sine = std::sin(half_angle);
    return {unit_axis.x * sine, unit_axis.y * sine, unit_axis.z * sine,
            std::cos(half_angle)};
}

Quat Quat::from_euler(float roll, float pitch, float yaw)
{
    const Quat rotation_z = from_axis_angle(Vec3::unit_z(), yaw);
    const Quat rotation_y = from_axis_angle(Vec3::unit_y(), pitch);
    const Quat rotation_x = from_axis_angle(Vec3::unit_x(), roll);
    return rotation_z * rotation_y * rotation_x;
}

float Quat::length() const
{
    return std::sqrt(length_squared());
}

Quat Quat::normalized() const
{
    const float len = length();
    FORGE_ASSERT_MSG(len > kEpsilon, "normalizing a near-zero quaternion");
    const float inverse_length = 1.0f / len;
    return {x * inverse_length, y * inverse_length, z * inverse_length, w * inverse_length};
}

Quat Quat::inverse() const
{
    const float norm = length_squared();
    FORGE_ASSERT_MSG(norm > kEpsilon * kEpsilon, "inverting a near-zero quaternion");
    const float inverse_norm = 1.0f / norm;
    const Quat conjugated = conjugate();
    return {conjugated.x * inverse_norm, conjugated.y * inverse_norm,
            conjugated.z * inverse_norm, conjugated.w * inverse_norm};
}

Vec3 Quat::rotate_vector(const Vec3& v) const
{
    // Optimized v' = q * v * q^-1 for unit quaternions:
    //   t = 2 * (q.xyz x v); v' = v + w * t + (q.xyz x t)
    const Vec3 axis{x, y, z};
    const Vec3 t = cross(axis, v) * 2.0f;
    return v + t * w + cross(axis, t);
}

Quat operator*(const Quat& a, const Quat& b)
{
    const Vec3 av{a.x, a.y, a.z};
    const Vec3 bv{b.x, b.y, b.z};
    const Vec3 v = bv * a.w + av * b.w + cross(av, bv);
    const float w = a.w * b.w - dot(av, bv);
    return {v.x, v.y, v.z, w};
}

Quat slerp(const Quat& a, const Quat& b, float t)
{
    float cosine = dot(a, b);

    // Take the shorter arc.
    Quat end = b;
    if (cosine < 0.0f)
    {
        cosine = -cosine;
        end = {-b.x, -b.y, -b.z, -b.w};
    }

    float weight_a;
    float weight_b;
    if (cosine > 1.0f - kEpsilon)
    {
        // Nearly parallel: fall back to normalized linear interpolation.
        weight_a = 1.0f - t;
        weight_b = t;
    }
    else
    {
        const float angle = std::acos(cosine);
        const float inverse_sine = 1.0f / std::sin(angle);
        weight_a = std::sin((1.0f - t) * angle) * inverse_sine;
        weight_b = std::sin(t * angle) * inverse_sine;
    }

    const Quat blended{a.x * weight_a + end.x * weight_b,
                       a.y * weight_a + end.y * weight_b,
                       a.z * weight_a + end.z * weight_b,
                       a.w * weight_a + end.w * weight_b};
    return blended.normalized();
}

} // namespace forge
