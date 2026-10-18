#include "pch.h"
#include "Skeleton.h"

namespace bdt
{
    Skeleton::Skeleton() = default;
    Skeleton::~Skeleton() = default;

    SkeletonUPtr Skeleton::Create(const List<ModelNode>& nodes)
    {
        SkeletonUPtr skeleton(new Skeleton());
        if (!skeleton->Init(nodes)) return nullptr;
        return skeleton;
    }

    bool Skeleton::Init(const List<ModelNode>& nodes)
    {
        for (usize i = 0; i < nodes.size(); ++i)
        {
            const String& name = nodes[i].name;
            if (!m_nodeNameToIndex.emplace(name, static_cast<int32>(i)).second)
                return false;
        }

        return true;
    }

    int32 Skeleton::GetNodeIndex(const String& name) const
    {
        auto it = m_nodeNameToIndex.find(name);
        if (it == m_nodeNameToIndex.end()) return -1;
        return it->second;
    }

    int32 Skeleton::GetBoneID(const String& name) const
    {
        auto it = m_boneNameToID.find(name);
        if (it == m_boneNameToID.end()) return -1;
        return static_cast<int32>(it->second);
    }

    int32 Skeleton::AddBone(const String& name, const glm::mat4& offset)
    {
        int32 existingID = GetBoneID(name);
        if (existingID >= 0) return existingID;

        int32 nodeIndex = GetNodeIndex(name);
        if (nodeIndex < 0) return -1;

        uint32 boneID = static_cast<uint32>(m_bones.size());
        ModelBone bone;
        bone.name = name;
        bone.id = boneID;
        bone.nodeIndex = nodeIndex;
        bone.offset = offset;

        m_bones.push_back(std::move(bone));
        m_boneNameToID.emplace(name, boneID);

        return static_cast<int32>(boneID);
    }

    const List<ModelBone>& Skeleton::GetBones() const
    {
        return m_bones;
    }

    uint32 Skeleton::GetBoneCount() const
    {
        return static_cast<uint32>(m_bones.size());
    }

    bool Skeleton::HasBones() const
    {
        return !m_bones.empty();
    }
}