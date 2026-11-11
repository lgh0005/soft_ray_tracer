#include "pch.h"
#include "AABB.h"

namespace bdt
{
    AABB::AABB(const vec3& min, const vec3& max) : m_min(min), m_max(max) { }
    AABB::~AABB() = default;

    void AABB::Expand(const vec3& point)
    {
        m_min = glm::min(m_min, point);
        m_max = glm::max(m_max, point);
    }

    void AABB::Expand(const AABB& other)
    {
        m_min = glm::min(m_min, other.m_min);
        m_max = glm::max(m_max, other.m_max);
    }

    vec3 AABB::GetCenter() const
    {
        return (m_min + m_max) * 0.5f;
    }

    vec3 AABB::GetExtent() const
    {
        return m_max - m_min;
    }

    const vec3& AABB::GetMin() const
    {
        return m_min;
    }

    const vec3& AABB::GetMax() const
    {
        return m_max;
    }

    bool AABB::Intersect
    (
        const vec3& rayOrigin,
        const vec3& rayDirection,
        float tMin,
        float tMax
    ) const
    {
        // X, Y, Z 축에 대해 각각 교차 구간을 계산
        for (int axis = 0; axis < 3; ++axis)
        {
            const float origin = rayOrigin[axis];
            const float direction = rayDirection[axis];
            const float minBound = m_min[axis];
            const float maxBound = m_max[axis];

            // 광선이 현재 축에 평행한 경우
            if (std::abs(direction) < Epsilon<float>)
            {
                // 해당 축의 경계 밖에 있으면 교차하지 않음
                if (origin < minBound || origin > maxBound) return false;
                continue;
            }

            const float invDirection = 1.0f / direction;
            float t0 = (minBound - origin) * invDirection;
            float t1 = (maxBound - origin) * invDirection;
            if (t0 > t1) std::swap(t0, t1);

            // 현재까지의 교차 구간과 새로운 구간의 교집합
            tMin = std::max(tMin, t0);
            tMax = std::min(tMax, t1);

            // 교차 구간이 존재하지 않으면 종료
            if (tMin > tMax)
                return false;
        }

        return true;
    }
}
