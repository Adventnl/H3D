#include "forge/math/mat4.hpp"

#include <cmath>

#include "forge/foundation/assert.hpp"
#include "forge/math/quat.hpp"

namespace forge
{

Mat4 Mat4::from_quat(const Quat& rotation)
{
    const float x = rotation.x;
    const float y = rotation.y;
    const float z = rotation.z;
    const float w = rotation.w;

    const float xx = x * x;
    const float yy = y * y;
    const float zz = z * z;
    const float xy = x * y;
    const float xz = x * z;
    const float yz = y * z;
    const float wx = w * x;
    const float wy = w * y;
    const float wz = w * z;

    Mat4 result;
    result.columns[0] = Vec4{1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f};
    result.columns[1] = Vec4{2.0f * (xy - wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx), 0.0f};
    result.columns[2] = Vec4{2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (xx + yy), 0.0f};
    result.columns[3] = Vec4{0.0f, 0.0f, 0.0f, 1.0f};
    return result;
}

Mat4 Mat4::perspective(float fov_y, float aspect_ratio, float near_plane, float far_plane)
{
    FORGE_ASSERT(fov_y > 0.0f && aspect_ratio > 0.0f);
    FORGE_ASSERT(near_plane > 0.0f && far_plane > near_plane);

    const float focal = 1.0f / std::tan(fov_y * 0.5f);
    const float depth_range = near_plane - far_plane;

    Mat4 result = Mat4::zero();
    result.columns[0].x = focal / aspect_ratio;
    result.columns[1].y = focal;
    result.columns[2].z = (far_plane + near_plane) / depth_range;
    result.columns[2].w = -1.0f;
    result.columns[3].z = (2.0f * far_plane * near_plane) / depth_range;
    return result;
}

Mat4 Mat4::orthographic(float left, float right, float bottom, float top,
                        float near_plane, float far_plane)
{
    FORGE_ASSERT(right != left && top != bottom && far_plane != near_plane);

    Mat4 result;
    result.columns[0].x = 2.0f / (right - left);
    result.columns[1].y = 2.0f / (top - bottom);
    result.columns[2].z = -2.0f / (far_plane - near_plane);
    result.columns[3] = Vec4{-(right + left) / (right - left),
                             -(top + bottom) / (top - bottom),
                             -(far_plane + near_plane) / (far_plane - near_plane), 1.0f};
    return result;
}

Mat4 Mat4::look_at(const Vec3& eye, const Vec3& target, const Vec3& up)
{
    const Vec3 forward = (target - eye).normalized();
    const Vec3 side = cross(forward, up).normalized();
    const Vec3 true_up = cross(side, forward);

    Mat4 result;
    result.columns[0] = Vec4{side.x, true_up.x, -forward.x, 0.0f};
    result.columns[1] = Vec4{side.y, true_up.y, -forward.y, 0.0f};
    result.columns[2] = Vec4{side.z, true_up.z, -forward.z, 0.0f};
    result.columns[3] = Vec4{-dot(side, eye), -dot(true_up, eye), dot(forward, eye), 1.0f};
    return result;
}

Mat4 Mat4::transposed() const
{
    Mat4 result;
    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 4; ++column)
        {
            result.set(column, row, at(row, column));
        }
    }
    return result;
}

Mat4 operator*(const Mat4& a, const Mat4& b)
{
    Mat4 result = Mat4::zero();
    for (std::size_t column = 0; column < 4; ++column)
    {
        const Vec4& bc = b.columns[column];
        result.columns[column] = a.columns[0] * bc.x + a.columns[1] * bc.y +
                                 a.columns[2] * bc.z + a.columns[3] * bc.w;
    }
    return result;
}

Vec4 operator*(const Mat4& m, const Vec4& v)
{
    return m.columns[0] * v.x + m.columns[1] * v.y + m.columns[2] * v.z +
           m.columns[3] * v.w;
}

Vec3 transform_point(const Mat4& m, const Vec3& point)
{
    return (m * Vec4{point, 1.0f}).xyz();
}

Vec3 transform_direction(const Mat4& m, const Vec3& direction)
{
    return (m * Vec4{direction, 0.0f}).xyz();
}

bool near_equal(const Mat4& a, const Mat4& b, float epsilon)
{
    for (std::size_t column = 0; column < 4; ++column)
    {
        if (!near_equal(a.columns[column], b.columns[column], epsilon))
        {
            return false;
        }
    }
    return true;
}

} // namespace forge
