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
}
