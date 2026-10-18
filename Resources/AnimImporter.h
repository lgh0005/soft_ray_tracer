#pragma once
#include "Importer.h"
#include "AnimData.h"

struct aiNodeAnim;

namespace bdt
{
    BDT_CLASS_PTR(AnimationImporter)

    class AnimationImporter final : public Importer
    {
        using Super = Importer;

    public:
        AnimationImporter();
        ~AnimationImporter() override;

        bool Init(const String& path);

        uint32 GetAnimationCount() const;
        uint32 GetChannelCount(uint32 animationIndex) const;

        // Independent Data Extraction
        bool ProcessAnimationInfo(uint32 animationIndex, AnimationData& output) const;
        bool ProcessChannel(uint32 animationIndex, uint32 channelIndex, AnimChannelData& output) const;

        // Complete Animation Extraction
        bool ProcessAnimation(uint32 animationIndex, AnimationData& output) const;

    private:
        static bool ExtractChannel(const aiNodeAnim* channel, AnimChannelData& output);
    };
}