#pragma once
#include "AnimData.h"

namespace bdt
{
    BDT_CLASS_PTR(Pose)
    BDT_CLASS_PTR(Animation)

    class Animation
    {
        BDT_DISABLE_COPY(Animation)
        BDT_DISABLE_MOVE(Animation)

    public:
        ~Animation();
        static AnimationPtr Create(AnimationData&& data);
        const AnimChannelData* FindChannel(const String& nodeName) const;
        Pose SamplePose
        (
            const AnimChannelData& channel,
            double time,
            const Pose& defaultPose
        ) const;

        const AnimationData& GetData() const;
        const String& GetName() const;
        double GetDuration() const;
        double GetTicksPerSecond() const;

    private:
        static vec3 SamplePosition(const List<AnimKeyPosition>& positions, double time, const vec3& defaultPosition);
        static quat SampleRotation(const List<AnimKeyRotation>& rotations, double time, const quat& defaultRotation);
        static vec3 SampleScale(const List<AnimKeyScale>& scales, double time, const vec3& defaultScale);

    private:
        Animation();
        bool Init(AnimationData&& data);

    private:
        AnimationData m_data;
        Dictionary<String, usize> m_channelIndices;
    };
}