#pragma once

// Unit quaternion for 3D rotations. Stored as (x, y, z, w) with w the scalar
// part; identity is (0, 0, 0, 1).

#include "forge/math/constants.hpp"
#include "forge/math/vec3.hpp"

namespace forge
{

struct Quat
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    constexpr Quat() = default;
    constexpr Quat(float x_value, float y_value, float z_value, float w_value)
        : x(x_value), y(y_value), z(z_value), w(w_value)
    {
    }

    [[nodiscard]] static constexpr Quat identity() { return {0.0f, 0.0f, 0.0f, 1.0f}; }

    /// Rotation of `angle_radians` around `axis`. The axis does not need to
    /// be normalized (a near-zero axis yields identity).
    [[nodiscard]] static Quat from_axis_angle(const Vec3& axis, float angle_radians);

    /// Rotation from Z-Y-X Euler angles (radians): yaw (Z), pitch (Y), roll (X).
    [[nodiscard]] static Quat from_euler(float roll, float pitch, float yaw);

    [[nodiscard]] constexpr float length_squared() const
    {
        return x * x + y * y + z * z + w * w;
    }

    [[nodiscard]] float length() const;

    [[nodiscard]] Quat normalized() const;

    [[nodiscard]] constexpr Quat conjugate() const { return {-x, -y, -z, w}; }

    /// Inverse rotation. Equals the conjugate for unit quaternions.
    [[nodiscard]] Quat inverse() const;

    /// Rotate a vector by this (unit) quaternion.
    [[nodiscard]] Vec3 rotate_vector(const Vec3& v) const;

    [[nodiscard]] constexpr bool operator==(const Quat&) const = default;
};

/// Hamilton product: applying (a * b) rotates by b first, then a.
[[nodiscard]] Quat operator*(const Quat& a, const Quat& b);

[[nodiscard]] constexpr float dot(const Quat& a, const Quat& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

/// Spherical linear interpolation between unit quaternions.
[[nodiscard]] Quat slerp(const Quat& a, const Quat& b, float t);

[[nodiscard]] constexpr bool near_equal(const Quat& a, const Quat& b, float epsilon = kEpsilon)
{
    return near_equal(a.x, b.x, epsilon) && near_equal(a.y, b.y, epsilon) &&
           near_equal(a.z, b.z, epsilon) && near_equal(a.w, b.w, epsilon);
}

} // namespace forge
