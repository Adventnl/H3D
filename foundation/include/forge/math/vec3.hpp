#pragma once

// 3D float vector.

#include <cmath>

#include "forge/foundation/assert.hpp"
#include "forge/math/constants.hpp"

namespace forge
{

struct Vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    constexpr Vec3() = default;
    constexpr Vec3(float x_value, float y_value, float z_value)
        : x(x_value), y(y_value), z(z_value)
    {
    }
    constexpr explicit Vec3(float scalar) : x(scalar), y(scalar), z(scalar) {}

    [[nodiscard]] static constexpr Vec3 zero() { return {0.0f, 0.0f, 0.0f}; }
    [[nodiscard]] static constexpr Vec3 one() { return {1.0f, 1.0f, 1.0f}; }
    [[nodiscard]] static constexpr Vec3 unit_x() { return {1.0f, 0.0f, 0.0f}; }
    [[nodiscard]] static constexpr Vec3 unit_y() { return {0.0f, 1.0f, 0.0f}; }
    [[nodiscard]] static constexpr Vec3 unit_z() { return {0.0f, 0.0f, 1.0f}; }

    constexpr Vec3& operator+=(const Vec3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    constexpr Vec3& operator-=(const Vec3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    constexpr Vec3& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr Vec3& operator/=(float scalar)
    {
        FORGE_ASSERT(scalar != 0.0f);
        const float inverse = 1.0f / scalar;
        x *= inverse;
        y *= inverse;
        z *= inverse;
        return *this;
    }

    [[nodiscard]] constexpr Vec3 operator-() const { return {-x, -y, -z}; }

    [[nodiscard]] constexpr float length_squared() const { return x * x + y * y + z * z; }

    [[nodiscard]] float length() const { return std::sqrt(length_squared()); }

    /// Unit-length copy. Asserts on (near-)zero vectors.
    [[nodiscard]] Vec3 normalized() const
    {
        const float len = length();
        FORGE_ASSERT_MSG(len > kEpsilon, "normalizing a near-zero Vec3");
        const float inverse = 1.0f / len;
        return {x * inverse, y * inverse, z * inverse};
    }

    /// Unit-length copy, or `fallback` when the vector is near zero.
    [[nodiscard]] Vec3 safe_normalized(const Vec3& fallback = zero()) const
    {
        const float len = length();
        if (len <= kEpsilon)
        {
            return fallback;
        }
        const float inverse = 1.0f / len;
        return {x * inverse, y * inverse, z * inverse};
    }

    [[nodiscard]] constexpr bool operator==(const Vec3&) const = default;
};

[[nodiscard]] constexpr Vec3 operator+(const Vec3& a, const Vec3& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

[[nodiscard]] constexpr Vec3 operator-(const Vec3& a, const Vec3& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

[[nodiscard]] constexpr Vec3 operator*(const Vec3& v, float scalar)
{
    return {v.x * scalar, v.y * scalar, v.z * scalar};
}

[[nodiscard]] constexpr Vec3 operator*(float scalar, const Vec3& v)
{
    return v * scalar;
}

/// Component-wise product (useful for scale operations).
[[nodiscard]] constexpr Vec3 operator*(const Vec3& a, const Vec3& b)
{
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

[[nodiscard]] constexpr Vec3 operator/(const Vec3& v, float scalar)
{
    Vec3 result = v;
    result /= scalar;
    return result;
}

[[nodiscard]] constexpr float dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

[[nodiscard]] constexpr Vec3 cross(const Vec3& a, const Vec3& b)
{
    return {a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
}

[[nodiscard]] inline float distance(const Vec3& a, const Vec3& b)
{
    return (b - a).length();
}

[[nodiscard]] constexpr Vec3 lerp(const Vec3& a, const Vec3& b, float t)
{
    return {lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t)};
}

[[nodiscard]] constexpr Vec3 min(const Vec3& a, const Vec3& b)
{
    return {a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z};
}

[[nodiscard]] constexpr Vec3 max(const Vec3& a, const Vec3& b)
{
    return {a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z};
}

[[nodiscard]] constexpr Vec3 clamp(const Vec3& value, const Vec3& low, const Vec3& high)
{
    return min(max(value, low), high);
}

[[nodiscard]] constexpr bool near_equal(const Vec3& a, const Vec3& b, float epsilon = kEpsilon)
{
    return near_equal(a.x, b.x, epsilon) && near_equal(a.y, b.y, epsilon) &&
           near_equal(a.z, b.z, epsilon);
}

} // namespace forge
