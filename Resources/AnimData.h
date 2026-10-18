#pragma once
#include <Core/Types.h>

namespace bdt
{
    struct AnimKeyPosition
    {
        double time{ 0.0 };
        vec3 position{ 0.0f };
    };

    struct AnimKeyRotation
    {
        double time{ 0.0 };
        quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
    };

    struct AnimKeyScale
    {
        double time{ 0.0 };
        vec3 scale{ 1.0f };
    };

    struct AnimChannelData
    {
        String nodeName;
        int32 nodeIndex{ -1 };
        List<AnimKeyPosition> positions;
        List<AnimKeyRotation> rotations;
        List<AnimKeyScale> scales;
    };

    struct AnimationData
    {
        String name;
        double duration{ 0.0 };
        double ticksPerSecond{ 0.0 };
        List<AnimChannelData> channels;
    };
}