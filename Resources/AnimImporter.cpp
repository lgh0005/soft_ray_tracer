#include "pch.h"
#include "AnimImporter.h"
#include <assimp/scene.h>

namespace bdt
{
    AnimationImporter::AnimationImporter() = default;
    AnimationImporter::~AnimationImporter() = default;

    bool AnimationImporter::Init(const String& path)
    {
        return Super::LoadScene(path, 0);
    }

    uint32 AnimationImporter::GetAnimationCount() const
    {
        const aiScene* scene = GetScene();
        return scene ? scene->mNumAnimations : 0;
    }

    uint32 AnimationImporter::GetChannelCount(uint32 animationIndex) const
    {
        const aiScene* scene = GetScene();
        if (!scene || animationIndex >= scene->mNumAnimations || !scene->mAnimations)
            return 0;

        const aiAnimation* animation = scene->mAnimations[animationIndex];
        return animation ? animation->mNumChannels : 0;
    }

    /*======================================//
    //   independent data extraction tasks  //
    //======================================*/

    bool AnimationImporter::ProcessAnimationInfo(uint32 animationIndex, AnimationData& output) const
    {
        const aiScene* scene = GetScene();
        if (!scene || animationIndex >= scene->mNumAnimations || !scene->mAnimations)
            return false;

        const aiAnimation* animation = scene->mAnimations[animationIndex];
        if (!animation || (animation->mNumChannels > 0 && !animation->mChannels))
            return false;

        // Animation Metadata
        output.name = animation->mName.C_Str();
        output.duration = animation->mDuration;
        output.ticksPerSecond = animation->mTicksPerSecond;

        return true;
    }

    bool AnimationImporter::ProcessChannel(uint32 animationIndex, uint32 channelIndex, AnimChannelData& output) const
    {
        const aiScene* scene = GetScene();
        if (!scene || animationIndex >= scene->mNumAnimations || !scene->mAnimations)
            return false;

        const aiAnimation* animation = scene->mAnimations[animationIndex];
        if (!animation || channelIndex >= animation->mNumChannels || !animation->mChannels)
            return false;

        return ExtractChannel(animation->mChannels[channelIndex], output);
    }

    bool AnimationImporter::ProcessAnimation(uint32 animationIndex, AnimationData& output) const
    {
        AnimationData data;
        if (!ProcessAnimationInfo(animationIndex, data))
            return false;

        // Allocate Animation Channels
        data.channels.resize(GetChannelCount(animationIndex));

        // Extract Animation Channels
        for (uint32 i = 0; i < static_cast<uint32>(data.channels.size()); ++i)
        {
            if (!ProcessChannel(animationIndex, i, data.channels[i]))
                return false;
        }

        output = std::move(data);
        return true;
    }

    /*=====================================//
    //   internal data extraction helpers  //
    //=====================================*/

    bool AnimationImporter::ExtractChannel(const aiNodeAnim* channel, AnimChannelData& output)
    {
        if (!channel) return false;
        if (channel->mNumPositionKeys > 0 && !channel->mPositionKeys) return false;
        if (channel->mNumRotationKeys > 0 && !channel->mRotationKeys) return false;
        if (channel->mNumScalingKeys > 0 && !channel->mScalingKeys) return false;

        AnimChannelData data;
        data.nodeName = channel->mNodeName.C_Str();
        data.nodeIndex = -1;

        // Position Keys
        data.positions.reserve(channel->mNumPositionKeys);

        for (uint32 i = 0; i < channel->mNumPositionKeys; ++i)
        {
            const aiVectorKey& source = channel->mPositionKeys[i];
            AnimKeyPosition key;
            key.time = source.mTime;
            key.position = ToVector(source.mValue);
            data.positions.push_back(std::move(key));
        }

        // Rotation Keys
        data.rotations.reserve(channel->mNumRotationKeys);

        for (uint32 i = 0; i < channel->mNumRotationKeys; ++i)
        {
            const aiQuatKey& source = channel->mRotationKeys[i];
            AnimKeyRotation key;
            key.time = source.mTime;
            key.rotation = ToQuaternion(source.mValue);
            data.rotations.push_back(std::move(key));
        }

        // Scale Keys
        data.scales.reserve(channel->mNumScalingKeys);

        for (uint32 i = 0; i < channel->mNumScalingKeys; ++i)
        {
            const aiVectorKey& source = channel->mScalingKeys[i];
            AnimKeyScale key;
            key.time = source.mTime;
            key.scale = ToVector(source.mValue);
            data.scales.push_back(std::move(key));
        }

        output = std::move(data);
        return true;
    }
}