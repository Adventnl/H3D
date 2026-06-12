#pragma once

// 3x3 float matrix, column-major. columns[c] is column c; at(row, col)
// addresses by mathematical convention.

#include <array>

#include "forge/math/vec3.hpp"

namespace forge
{

struct Mat3
{
    // Defaults to identity.
    std::array<Vec3, 3> columns{Vec3{1.0f, 0.0f, 0.0f},
                                Vec3{0.0f, 1.0f, 0.0f},
                                Vec3{0.0f, 0.0f, 1.0f}};

    constexpr Mat3() = default;

    constexpr Mat3(const Vec3& column0, const Vec3& column1, const Vec3& column2)
        : columns{column0, column1, column2}
    {
    }

    [[nodiscard]] static constexpr Mat3 identity() { return Mat3{}; }

    [[nodiscard]] static constexpr Mat3 zero()
    {
        return Mat3{Vec3::zero(), Vec3::zero(), Vec3::zero()};
    }

    [[nodiscard]] constexpr float at(int row, int column) const
    {
        const Vec3& c = columns[static_cast<std::size_t>(column)];
        return row == 0 ? c.x : (row == 1 ? c.y : c.z);
    }

    constexpr void set(int row, int column, float value)
    {
        Vec3& c = columns[static_cast<std::size_t>(column)];
        if (row == 0)
        {
            c.x = value;
        }
        else if (row == 1)
        {
            c.y = value;
        }
        else
        {
            c.z = value;
        }
    }

    [[nodiscard]] Mat3 transposed() const;
    [[nodiscard]] float determinant() const;

    [[nodiscard]] constexpr bool operator==(const Mat3&) const = default;
};

[[nodiscard]] Mat3 operator*(const Mat3& a, const Mat3& b);
[[nodiscard]] Vec3 operator*(const Mat3& m, const Vec3& v);

[[nodiscard]] bool near_equal(const Mat3& a, const Mat3& b, float epsilon = kEpsilon);

} // namespace forge
