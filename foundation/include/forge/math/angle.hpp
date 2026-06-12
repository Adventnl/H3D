#pragma once

// Angle conversion and wrapping helpers. Angles are plain floats in radians
// throughout the math library; degrees only appear at API boundaries.

#include <cmath>

#include "forge/math/constants.hpp"

namespace forge
{

[[nodiscard]] constexpr float to_radians(float degrees) noexcept
{
    return degrees * kDegToRad;
}

[[nodiscard]] constexpr float to_degrees(float radians) noexcept
{
    return radians * kRadToDeg;
}

/// Wrap an angle into [-pi, pi).
[[nodiscard]] inline float wrap_pi(float radians) noexcept
{
    float wrapped = std::fmod(radians + kPi, kTwoPi);
    if (wrapped < 0.0f)
    {
        wrapped += kTwoPi;
    }
    return wrapped - kPi;
}

/// Wrap an angle into [0, 2*pi).
[[nodiscard]] inline float wrap_two_pi(float radians) noexcept
{
    float wrapped = std::fmod(radians, kTwoPi);
    if (wrapped < 0.0f)
    {
        wrapped += kTwoPi;
    }
    return wrapped;
}

} // namespace forge
