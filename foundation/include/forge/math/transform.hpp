#pragma once

// Decomposed TRS transform: translation, rotation, scale.
//
// Composition and inversion treat the components independently, which is
// exact for uniform scale. Combining rotation with non-uniform scale across
// hierarchy levels introduces shear that a TRS cannot represent; later
// phases handle that at the matrix level.

#include "forge/math/mat4.hpp"
#include "forge/math/quat.hpp"
#include "forge/math/vec3.hpp"

namespace forge
{

struct Transform
{
    Vec3 translation = Vec3::zero();
    Quat rotation = Quat::identity();
    Vec3 scale = Vec3::one();

    constexpr Transform() = default;

    constexpr Transform(const Vec3& translation_value, const Quat& rotation_value,
                        const Vec3& scale_value)
        : translation(translation_value), rotation(rotation_value), scale(scale_value)
    {
    }

    [[nodiscard]] static constexpr Transform identity() { return Transform{}; }

    /// Equivalent matrix: T * R * S.
    [[nodiscard]] Mat4 to_matrix() const;

    /// Apply scale, then rotation, then translation.
    [[nodiscard]] Vec3 transform_point(const Vec3& point) const;

    /// Apply rotation only (directions ignore translation and scale).
    [[nodiscard]] Vec3 transform_direction(const Vec3& direction) const;

    /// Inverse transform (exact for uniform scale).
    [[nodiscard]] Transform inverse() const;

    [[nodiscard]] constexpr bool operator==(const Transform&) const = default;
};

/// parent * child: the result maps child-local space into parent's space.
[[nodiscard]] Transform combine(const Transform& parent, const Transform& child);

[[nodiscard]] bool near_equal(const Transform& a, const Transform& b,
                              float epsilon = kEpsilon);

} // namespace forge
