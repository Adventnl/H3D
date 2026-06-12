#pragma once

// 4D float vector (homogeneous coordinates, colors, matrix columns).

#include <cmath>

#include "forge/foundation/assert.hpp"
#include "forge/math/constants.hpp"
#include "forge/math/vec3.hpp"

namespace forge
{

struct Vec4
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    constexpr Vec4() = default;
    constexpr Vec4(float x_value, float y_value, float z_value, float w_value)
        : x(x_value), y(y_value), z(z_value), w(w_value)
    {
    }
    constexpr explicit Vec4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
    constexpr Vec4(const Vec3& xyz, float w_value) : x(xyz.x), y(xyz.y), z(xyz.z), w(w_value) {}

    [[nodiscard]] static constexpr Vec4 zero() { return {0.0f, 0.0f, 0.0f, 0.0f}; }
    [[nodiscard]] static constexpr Vec4 one() { return {1.0f, 1.0f, 1.0f, 1.0f}; }

    /// Drop the w component.
    [[nodiscard]] constexpr Vec3 xyz() const { return {x, y, z}; }

    constexpr Vec4& operator+=(const Vec4& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    constexpr Vec4& operator-=(const Vec4& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    constexpr Vec4& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    constexpr Vec4& operator/=(float scalar)
    {
        FORGE_ASSERT(scalar != 0.0f);
        const float inverse = 1.0f / scalar;
        x *= inverse;
        y *= inverse;
        z *= inverse;
        w *= inverse;
        return *this;
    }

    [[nodiscard]] constexpr Vec4 operator-() const { return {-x, -y, -z, -w}; }

    [[nodiscard]] constexpr float length_squared() const
    {
        return x * x + y * y + z * z + w * w;
    }

    [[nodiscard]] float length() const { return std::sqrt(length_squared()); }

    [[nodiscard]] constexpr bool operator==(const Vec4&) const = default;
};

[[nodiscard]] constexpr Vec4 operator+(const Vec4& a, const Vec4& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

[[nodiscard]] constexpr Vec4 operator-(const Vec4& a, const Vec4& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

[[nodiscard]] constexpr Vec4 operator*(const Vec4& v, float scalar)
{
    return {v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar};
}

[[nodiscard]] constexpr Vec4 operator*(float scalar, const Vec4& v)
{
    return v * scalar;
}

[[nodiscard]] constexpr Vec4 operator/(const Vec4& v, float scalar)
{
    Vec4 result = v;
    result /= scalar;
    return result;
}

[[nodiscard]] constexpr float dot(const Vec4& a, const Vec4& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

[[nodiscard]] constexpr bool near_equal(const Vec4& a, const Vec4& b, float epsilon = kEpsilon)
{
    return near_equal(a.x, b.x, epsilon) && near_equal(a.y, b.y, epsilon) &&
           near_equal(a.z, b.z, epsilon) && near_equal(a.w, b.w, epsilon);
}

} // namespace forge
