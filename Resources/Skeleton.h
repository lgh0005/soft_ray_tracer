#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Core/GLVertices.h>
#include "ModelData.h"

namespace bdt
{
    BDT_CLASS_PTR(Skeleton)

    class Skeleton
    {
        BDT_DISABLE_COPY(Skeleton)
        BDT_DISABLE_MOVE(Skeleton)

    public:
        ~Skeleton();
        static SkeletonUPtr Create(const List<ModelNode>& nodes);

        int32 GetNodeIndex(const String& name) const;
        int32 GetBoneID(const String& name) const;
        int32 AddBone(const String& name, const glm::mat4& offset);

        const List<ModelBone>& GetBones() const;
        uint32 GetBoneCount() const;
        bool HasBones() const;

        static void AddBoneWeight(GLSkinnedVertex& vertex, int32 boneID, float weight);
        static void NormalizeBoneWeights(GLSkinnedVertex& vertex);

    private:
        Skeleton();
        bool Init(const List<ModelNode>& nodes);

    private:
        Dictionary<String, int32> m_nodeNameToIndex;
        Dictionary<String, uint32> m_boneNameToID;
        List<ModelBone> m_bones;
    };
}