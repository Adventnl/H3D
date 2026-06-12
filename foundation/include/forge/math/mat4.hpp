#pragma once

// 4x4 float matrix, column-major (like OpenGL/GLM). columns[c] is column c.
//
// Conventions: right-handed coordinate system; projection matrices map depth
// to OpenGL-style [-1, 1] clip space. These conventions are fixed here in
// Phase 1 so every later system (viewport, renderer) agrees.

#include <array>

#include "forge/math/vec3.hpp"
#include "forge/math/vec4.hpp"

namespace forge
{

struct Quat;

struct Mat4
{
    // Defaults to identity.
    std::array<Vec4, 4> columns{Vec4{1.0f, 0.0f, 0.0f, 0.0f},
                                Vec4{0.0f, 1.0f, 0.0f, 0.0f},
                                Vec4{0.0f, 0.0f, 1.0f, 0.0f},
                                Vec4{0.0f, 0.0f, 0.0f, 1.0f}};

    constexpr Mat4() = default;

    constexpr Mat4(const Vec4& column0, const Vec4& column1, const Vec4& column2,
                   const Vec4& column3)
        : columns{column0, column1, column2, column3}
    {
    }

    [[nodiscard]] static constexpr Mat4 identity() { return Mat4{}; }

    [[nodiscard]] static constexpr Mat4 zero()
    {
        return Mat4{Vec4::zero(), Vec4::zero(), Vec4::zero(), Vec4::zero()};
    }

    [[nodiscard]] static constexpr Mat4 translation(const Vec3& offset)
    {
        Mat4 result;
        result.columns[3] = Vec4{offset, 1.0f};
        return result;
    }

    [[nodiscard]] static constexpr Mat4 scale(const Vec3& factors)
    {
        Mat4 result;
        result.columns[0].x = factors.x;
        result.columns[1].y = factors.y;
        result.columns[2].z = factors.z;
        return result;
    }

    /// Rotation matrix from a (unit) quaternion.
    [[nodiscard]] static Mat4 from_quat(const Quat& rotation);

    /// Right-handed perspective projection. fov_y is in radians.
    [[nodiscard]] static Mat4 perspective(float fov_y, float aspect_ratio,
                                          float near_plane, float far_plane);

    /// Right-handed orthographic projection.
    [[nodiscard]] static Mat4 orthographic(float left, float right, float bottom,
                                           float top, float near_plane, float far_plane);

    /// Right-handed view matrix looking from `eye` toward `target`.
    [[nodiscard]] static Mat4 look_at(const Vec3& eye, const Vec3& target, const Vec3& up);

    [[nodiscard]] constexpr float at(int row, int column) const
    {
        const Vec4& c = columns[static_cast<std::size_t>(column)];
        switch (row)
        {
        case 0: return c.x;
        case 1: return c.y;
        case 2: return c.z;
        default: return c.w;
        }
    }

    constexpr void set(int row, int column, float value)
    {
        Vec4& c = columns[static_cast<std::size_t>(column)];
        switch (row)
        {
        case 0: c.x = value; break;
        case 1: c.y = value; break;
        case 2: c.z = value; break;
        default: c.w = value; break;
        }
    }

    [[nodiscard]] Mat4 transposed() const;

    [[nodiscard]] constexpr bool operator==(const Mat4&) const = default;
};

[[nodiscard]] Mat4 operator*(const Mat4& a, const Mat4& b);
[[nodiscard]] Vec4 operator*(const Mat4& m, const Vec4& v);

/// Transform a position (w = 1). Assumes an affine matrix.
[[nodiscard]] Vec3 transform_point(const Mat4& m, const Vec3& point);

/// Transform a direction (w = 0); translation is ignored.
[[nodiscard]] Vec3 transform_direction(const Mat4& m, const Vec3& direction);

[[nodiscard]] bool near_equal(const Mat4& a, const Mat4& b, float epsilon = kEpsilon);

} // namespace forge
