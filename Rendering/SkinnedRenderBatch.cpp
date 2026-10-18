#include "pch.h"
#include "SkinnedRenderBatch.h"

namespace bdt
{
    SkinnedRenderBatch::SkinnedRenderBatch(Mesh* mesh, Material* material)
        : Super(mesh, material) { }

    SkinnedRenderBatch::~SkinnedRenderBatch() = default;

    void SkinnedRenderBatch::Clear()
    {
        m_properties.clear();
        m_boneMatrices.clear();
    }

    uint32 SkinnedRenderBatch::GetInstanceCount() const
    {
        return static_cast<uint32>(m_properties.size());
    }

    void SkinnedRenderBatch::Add
    (
        const SkinnedInstanceProperty& property,
        const List<mat4>& bones
    )
    {
        auto& instance = m_properties.emplace_back(property);
        instance.boneOffset = static_cast<uint32>(m_boneMatrices.size());
        instance.boneCount = static_cast<uint32>(bones.size());
        m_boneMatrices.insert(m_boneMatrices.end(), bones.begin(), bones.end());
    }

    const List<SkinnedInstanceProperty>& SkinnedRenderBatch::GetProperties() const
    {
        return m_properties;
    }

    const List<mat4>& SkinnedRenderBatch::GetBoneMatrices() const
    {
        return m_boneMatrices;
    }
}
