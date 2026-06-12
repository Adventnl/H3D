#pragma once

// 2D float vector.

#include <cmath>

#include "forge/foundation/assert.hpp"
#include "forge/math/constants.hpp"

namespace forge
{

struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;

    constexpr Vec2() = default;
    constexpr Vec2(float x_value, float y_value) : x(x_value), y(y_value) {}
    constexpr explicit Vec2(float scalar) : x(scalar), y(scalar) {}

    [[nodiscard]] static constexpr Vec2 zero() { return {0.0f, 0.0f}; }
    [[nodiscard]] static constexpr Vec2 one() { return {1.0f, 1.0f}; }

    constexpr Vec2& operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vec2& operator-=(const Vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr Vec2& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr Vec2& operator/=(float scalar)
    {
        FORGE_ASSERT(scalar != 0.0f);
        const float inverse = 1.0f / scalar;
        x *= inverse;
        y *= inverse;
        return *this;
    }

    [[nodiscard]] constexpr Vec2 operator-() const { return {-x, -y}; }

    [[nodiscard]] constexpr float length_squared() const { return x * x + y * y; }

    [[nodiscard]] float length() const { return std::sqrt(length_squared()); }

    /// Unit-length copy. Asserts on (near-)zero vectors.
    [[nodiscard]] Vec2 normalized() const
    {
        const float len = length();
        FORGE_ASSERT_MSG(len > kEpsilon, "normalizing a near-zero Vec2");
        const float inverse = 1.0f / len;
        return {x * inverse, y * inverse};
    }

    /// Unit-length copy, or `fallback` when the vector is near zero.
    [[nodiscard]] Vec2 safe_normalized(const Vec2& fallback = zero()) const
    {
        const float len = length();
        if (len <= kEpsilon)
        {
            return fallback;
        }
        const float inverse = 1.0f / len;
        return {x * inverse, y * inverse};
    }

    [[nodiscard]] constexpr bool operator==(const Vec2&) const = default;
};

[[nodiscard]] constexpr Vec2 operator+(const Vec2& a, const Vec2& b)
{
    return {a.x + b.x, a.y + b.y};
}

[[nodiscard]] constexpr Vec2 operator-(const Vec2& a, const Vec2& b)
{
    return {a.x - b.x, a.y - b.y};
}

[[nodiscard]] constexpr Vec2 operator*(const Vec2& v, float scalar)
{
    return {v.x * scalar, v.y * scalar};
}

[[nodiscard]] constexpr Vec2 operator*(float scalar, const Vec2& v)
{
    return v * scalar;
}

[[nodiscard]] constexpr Vec2 operator/(const Vec2& v, float scalar)
{
    Vec2 result = v;
    result /= scalar;
    return result;
}

[[nodiscard]] constexpr float dot(const Vec2& a, const Vec2& b)
{
    return a.x * b.x + a.y * b.y;
}

[[nodiscard]] inline float distance(const Vec2& a, const Vec2& b)
{
    return (b - a).length();
}

[[nodiscard]] constexpr Vec2 lerp(const Vec2& a, const Vec2& b, float t)
{
    return {lerp(a.x, b.x, t), lerp(a.y, b.y, t)};
}

[[nodiscard]] constexpr bool near_equal(const Vec2& a, const Vec2& b, float epsilon = kEpsilon)
{
    return near_equal(a.x, b.x, epsilon) && near_equal(a.y, b.y, epsilon);
}

} // namespace forge
