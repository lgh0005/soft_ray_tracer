#include "pch.h"
#include "StaticRenderBatch.h"

namespace bdt
{
    StaticRenderBatch::StaticRenderBatch(Mesh* mesh, Material* material)
        : Super(mesh, material) { }

    StaticRenderBatch::~StaticRenderBatch() = default;

    void StaticRenderBatch::Clear()
    {
        m_properties.clear();
    }

    uint32 StaticRenderBatch::GetInstanceCount() const
    {
        return static_cast<uint32>(m_properties.size());
    }

    void StaticRenderBatch::Add(const StaticInstanceProperty& property)
    {
        m_properties.push_back(property);
    }

    const List<StaticInstanceProperty>& StaticRenderBatch::GetProperties() const
    {
        return m_properties;
    }
}
