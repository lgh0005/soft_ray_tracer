#include "pch.h"
#include "Animation.h"
#include "Pose.h"

namespace bdt
{
    Animation::Animation() = default;
    Animation::~Animation() = default;

    AnimationPtr Animation::Create(AnimationData&& data)
    {
        AnimationPtr animation(new Animation());
        if (!animation->Init(std::move(data))) return nullptr;
        return animation;
    }

    bool Animation::Init(AnimationData&& data)
    {
        m_data = std::move(data);

        m_channelIndices.clear();
        m_channelIndices.reserve(m_data.channels.size());
        for (usize i = 0; i < m_data.channels.size(); ++i)
            m_channelIndices.emplace(m_data.channels[i].nodeName, i);

        return true;
    }

    const AnimChannelData* Animation::FindChannel(const String& nodeName) const
    {
        auto it = m_channelIndices.find(nodeName);
        if (it == m_channelIndices.end()) return nullptr;
        return &m_data.channels[it->second];
    }

    Pose Animation::SamplePose
    (
        const AnimChannelData& channel,
        double time,
        const Pose& defaultPose
    ) const
    {
        const vec3 position = SamplePosition(channel.positions, time, defaultPose.GetPosition());
        const quat rotation = SampleRotation(channel.rotations, time, defaultPose.GetRotation());
        const vec3 scale = SampleScale(channel.scales, time, defaultPose.GetScale());
        return Pose(position, rotation, scale);
    }

    vec3 Animation::SamplePosition
    (
        const List<AnimKeyPosition>& positions,
        double time,
        const vec3& defaultPosition
    )
    {
        if (positions.empty()) return defaultPosition;
        if (time <= positions.front().time) return positions.front().position;
        if (time >= positions.back().time) return positions.back().position;

        for (usize i = 0; i + 1 < positions.size(); ++i)
        {
            const auto& posA = positions[i];
            const auto& posB = positions[i + 1];
            if (time > posB.time) continue;

            const double duration = posB.time - posA.time;
            float factor = 0.0f;
            if (duration > 0.0)
                factor = (float)std::clamp((time - posA.time) / duration, 0.0, 1.0);

            return glm::mix(posA.position, posB.position, factor);
        }

        return positions.back().position;
    }

    quat Animation::SampleRotation
    (
        const List<AnimKeyRotation>& rotations,
        double time,
        const quat& defaultRotation
    )
    {
        if (rotations.empty()) return defaultRotation;
        if (time <= rotations.front().time) return rotations.front().rotation;
        if (time >= rotations.back().time) return rotations.back().rotation;

        for (usize i = 0; i + 1 < rotations.size(); ++i)
        {
            const auto& rotA = rotations[i];
            const auto& rotB = rotations[i + 1];
            if (time > rotB.time) continue;

            const double duration = rotB.time - rotA.time;
            float factor = 0.0f;
            if (duration > 0.0)
                factor = (float)std::clamp((time - rotA.time) / duration, 0.0, 1.0);

            return glm::normalize(glm::slerp(rotA.rotation, rotB.rotation, factor));
        }

        return rotations.back().rotation;
    }

    vec3 Animation::SampleScale
    (
        const List<AnimKeyScale>& scales,
        double time,
        const vec3& defaultScale
    )
    {
        if (scales.empty()) return defaultScale;
        if (time <= scales.front().time) return scales.front().scale;
        if (time >= scales.back().time) return scales.back().scale;

        for (usize i = 0; i + 1 < scales.size(); ++i)
        {
            const auto& scaleA = scales[i];
            const auto& scaleB = scales[i + 1];
            if (time > scaleB.time) continue;

            const double duration = scaleB.time - scaleA.time;
            float factor = 0.0f;

            if (duration > 0.0)
                factor = (float)std::clamp((time - scaleA.time) / duration, 0.0, 1.0);

            return glm::mix(scaleA.scale, scaleB.scale, factor);
        }

        return scales.back().scale;
    }

    const AnimationData& Animation::GetData() const
    {
        return m_data;
    }

    const String& Animation::GetName() const
    {
        return m_data.name;
    }

    double Animation::GetDuration() const
    {
        return m_data.duration;
    }

    double Animation::GetTicksPerSecond() const
    {
        return m_data.ticksPerSecond;
    }
}