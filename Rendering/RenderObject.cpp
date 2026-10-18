#include "pch.h"
#include "RenderObject.h"

namespace bdt
{
    RenderObject::RenderObject
    (
        MeshPtr mesh,
        const ResourceHandle<Material>& material,
        const mat4& worldMatrix,
        bool isSkinned,
        const List<mat4>* boneMatrices
    )
        : m_mesh(std::move(mesh))
        , m_material(material)
        , m_worldMatrix(worldMatrix)
        , m_isSkinned(isSkinned)
        , m_boneMatrices(boneMatrices)
    {}

    RenderObject::~RenderObject() = default;

    bool RenderObject::IsSkinned() const
    {
        return m_isSkinned;
    }

    const List<mat4>* RenderObject::GetBoneMatrices() const
    {
        return m_boneMatrices;
    }

    void RenderObject::SetWorldMatrix(const mat4& matrix)
    {
        m_worldMatrix = matrix;
    }

    const MeshPtr& RenderObject::GetMesh() const
    {
        return m_mesh;
    }

    const ResourceHandle<Material>& RenderObject::GetMaterial() const
    {
        return m_material;
    }

    const mat4& RenderObject::GetWorldMatrix() const
    {
        return m_worldMatrix;
    }
}