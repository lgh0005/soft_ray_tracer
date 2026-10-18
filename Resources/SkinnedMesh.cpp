#include "pch.h"
#include "SkinnedMesh.h"

namespace bdt
{
    SkinnedMesh::SkinnedMesh() = default;
    SkinnedMesh::~SkinnedMesh() = default;

    SkinnedMeshPtr SkinnedMesh::Create
    (
        const List<GLSkinnedVertex>& vertices, 
        const List<uint32>& indices
    )
    {
        SkinnedMeshPtr mesh(new SkinnedMesh());

        mesh->m_glMesh = GLSkinnedMesh::Create(vertices, indices);
        if (!mesh->m_glMesh) return nullptr;
        mesh->m_vertices = vertices;
        mesh->m_indices = indices;

        return mesh;
    }

    const GLMesh* SkinnedMesh::GetGLMesh() const
    {
        return m_glMesh.get();
    }

    const List<GLSkinnedVertex>& SkinnedMesh::GetVertices() const
    {
        return m_vertices;
    }

    const List<uint32>& SkinnedMesh::GetIndices() const
    {
        return m_indices;
    }
}