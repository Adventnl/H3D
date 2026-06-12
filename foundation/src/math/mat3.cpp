#include "forge/math/mat3.hpp"

namespace forge
{

Mat3 Mat3::transposed() const
{
    Mat3 result;
    for (int row = 0; row < 3; ++row)
    {
        for (int column = 0; column < 3; ++column)
        {
            result.set(column, row, at(row, column));
        }
    }
    return result;
}

float Mat3::determinant() const
{
    const Vec3& a = columns[0];
    const Vec3& b = columns[1];
    const Vec3& c = columns[2];
    return dot(a, cross(b, c));
}

Mat3 operator*(const Mat3& a, const Mat3& b)
{
    Mat3 result = Mat3::zero();
    for (std::size_t column = 0; column < 3; ++column)
    {
        const Vec3& bc = b.columns[column];
        result.columns[column] =
            a.columns[0] * bc.x + a.columns[1] * bc.y + a.columns[2] * bc.z;
    }
    return result;
}

Vec3 operator*(const Mat3& m, const Vec3& v)
{
    return m.columns[0] * v.x + m.columns[1] * v.y + m.columns[2] * v.z;
}

bool near_equal(const Mat3& a, const Mat3& b, float epsilon)
{
    return near_equal(a.columns[0], b.columns[0], epsilon) &&
           near_equal(a.columns[1], b.columns[1], epsilon) &&
           near_equal(a.columns[2], b.columns[2], epsilon);
}

} // namespace forge
