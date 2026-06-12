#include "forge/math/mat3.hpp"
#include "test_framework.hpp"

using forge::Mat3;
using forge::Vec3;

FORGE_TEST_CASE("math.mat3_identity")
{
    const Mat3 identity = Mat3::identity();
    const Vec3 v{1.0f, -2.0f, 3.0f};
    FORGE_CHECK(near_equal(identity * v, v));
    FORGE_CHECK_NEAR(identity.at(0, 0), 1.0f, 1e-6f);
    FORGE_CHECK_NEAR(identity.at(1, 0), 0.0f, 1e-6f);
    FORGE_CHECK_NEAR(identity.determinant(), 1.0f, 1e-6f);
}

FORGE_TEST_CASE("math.mat3_multiply")
{
    // Multiplying by identity is a no-op.
    const Mat3 m{Vec3{1.0f, 2.0f, 3.0f}, Vec3{4.0f, 5.0f, 6.0f}, Vec3{7.0f, 8.0f, 10.0f}};
    FORGE_CHECK(near_equal(m * Mat3::identity(), m));
    FORGE_CHECK(near_equal(Mat3::identity() * m, m));

    // Associativity with a vector: (a*b)*v == a*(b*v).
    const Mat3 a{Vec3{2.0f, 0.0f, 0.0f}, Vec3{0.0f, 3.0f, 0.0f}, Vec3{0.0f, 0.0f, 4.0f}};
    const Vec3 v{1.0f, 1.0f, 1.0f};
    FORGE_CHECK(near_equal((a * m) * v, a * (m * v), 1e-4f));
}

FORGE_TEST_CASE("math.mat3_transpose")
{
    const Mat3 m{Vec3{1.0f, 2.0f, 3.0f}, Vec3{4.0f, 5.0f, 6.0f}, Vec3{7.0f, 8.0f, 9.0f}};
    const Mat3 t = m.transposed();
    for (int row = 0; row < 3; ++row)
    {
        for (int column = 0; column < 3; ++column)
        {
            FORGE_CHECK_NEAR(t.at(row, column), m.at(column, row), 1e-6f);
        }
    }
    FORGE_CHECK(near_equal(t.transposed(), m));
}
