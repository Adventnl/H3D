#pragma once

// Math constants and scalar helpers shared by all math types.

namespace forge
{

inline constexpr float kPi = 3.14159265358979323846f;
inline constexpr float kTwoPi = 2.0f * kPi;
inline constexpr float kHalfPi = 0.5f * kPi;
inline constexpr float kEpsilon = 1e-5f;
inline constexpr float kDegToRad = kPi / 180.0f;
inline constexpr float kRadToDeg = 180.0f / kPi;

[[nodiscard]] constexpr float abs_value(float value) noexcept
{
    return value < 0.0f ? -value : value;
}

[[nodiscard]] constexpr bool near_equal(float a, float b, float epsilon = kEpsilon) noexcept
{
    return abs_value(a - b) <= epsilon;
}

[[nodiscard]] constexpr float clamp(float value, float low, float high) noexcept
{
    return value < low ? low : (value > high ? high : value);
}

[[nodiscard]] constexpr float lerp(float a, float b, float t) noexcept
{
    return a + (b - a) * t;
}

[[nodiscard]] constexpr float saturate(float value) noexcept
{
    return clamp(value, 0.0f, 1.0f);
}

} // namespace forge
