#include "pch.h"
#include "RenderBatch.h"

namespace bdt
{
    RenderBatch::RenderBatch(Mesh* mesh, Material* material)
        : m_mesh(mesh), m_material(material) { }

    RenderBatch::~RenderBatch() = default;

    Mesh* RenderBatch::GetMesh() const
    {
        return m_mesh;
    }

    Material* RenderBatch::GetMaterial() const
    {
        return m_material;
    }

    uint32 RenderBatch::GetBaseInstance() const
    {
        return m_baseInstance;
    }

    void RenderBatch::SetBaseInstance(uint32 baseInstance)
    {
        m_baseInstance = baseInstance;
    }

    uint32 RenderBatch::GetShadowBaseInstance() const
    {
        return m_shadowBaseInstance;
    }

    void RenderBatch::SetShadowBaseInstance(uint32 baseInstance)
    {
        m_shadowBaseInstance = baseInstance;
    }

    uint32 RenderBatch::GetShadowInstanceCount() const
    {
        return m_shadowInstanceCount;
    }

    void RenderBatch::SetShadowInstanceCount(uint32 count)
    {
        m_shadowInstanceCount = count;
    }
}
