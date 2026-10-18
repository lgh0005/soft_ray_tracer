#include "pch.h"
#include "Pose.h"

namespace bdt
{
    Pose::Pose
    (
        const vec3& position,
        const quat& rotation,
        const vec3& scale
    )
        : m_position(position)
        , m_rotation(rotation)
        , m_scale(scale) { }

    Pose::~Pose() = default;

    Pose::operator mat4() const
    {
        mat4 result = glm::mat4_cast(m_rotation);

        // 회전 행렬의 각 기저 벡터에 Scale 적용
        result[0] *= m_scale.x;
        result[1] *= m_scale.y;
        result[2] *= m_scale.z;

        // Translation 적용
        result[3] = vec4(m_position, 1.0f);

        return result;
    }

    Pose Pose::Interpolate(const Pose& poseA,  const Pose& poseB, float factor)
    {
        factor = glm::clamp(factor, 0.0f, 1.0f);
        return Pose
        (
            glm::mix(poseA.m_position, poseB.m_position, factor),
            glm::normalize(glm::slerp(poseA.m_rotation, poseB.m_rotation, factor)),
            glm::mix(poseA.m_scale, poseB.m_scale, factor)
        );
    }
}