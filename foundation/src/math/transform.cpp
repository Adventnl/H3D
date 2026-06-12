#include "forge/math/transform.hpp"

namespace forge
{

Mat4 Transform::to_matrix() const
{
    return Mat4::translation(translation) * Mat4::from_quat(rotation) * Mat4::scale(scale);
}

Vec3 Transform::transform_point(const Vec3& point) const
{
    return rotation.rotate_vector(point * scale) + translation;
}

Vec3 Transform::transform_direction(const Vec3& direction) const
{
    return rotation.rotate_vector(direction);
}

Transform Transform::inverse() const
{
    FORGE_ASSERT_MSG(abs_value(scale.x) > kEpsilon && abs_value(scale.y) > kEpsilon &&
                         abs_value(scale.z) > kEpsilon,
                     "inverting a transform with zero scale");

    Transform result;
    result.scale = Vec3{1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z};
    result.rotation = rotation.inverse();
    result.translation = result.rotation.rotate_vector(-translation) * result.scale;
    return result;
}

Transform combine(const Transform& parent, const Transform& child)
{
    Transform result;
    result.scale = parent.scale * child.scale;
    result.rotation = parent.rotation * child.rotation;
    result.translation = parent.transform_point(child.translation);
    return result;
}

bool near_equal(const Transform& a, const Transform& b, float epsilon)
{
    return near_equal(a.translation, b.translation, epsilon) &&
           near_equal(a.rotation, b.rotation, epsilon) &&
           near_equal(a.scale, b.scale, epsilon);
}

} // namespace forge
