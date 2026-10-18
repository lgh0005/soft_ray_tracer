#include "pch.h"
#include "StaticMesh.h"

namespace bdt
{
	StaticMesh::StaticMesh() = default;
	StaticMesh::~StaticMesh() = default;

    StaticMeshPtr StaticMesh::Create
    (
        const List<GLStaticVertex>& vertices, 
        const List<uint32>& indices
    )
    {
        StaticMeshPtr mesh(new StaticMesh());

        mesh->m_glMesh = GLStaticMesh::Create(vertices, indices);
        if (!mesh->m_glMesh) return nullptr;
        mesh->m_vertices = vertices;
        mesh->m_indices = indices;

        return mesh;
    }

    const GLMesh* StaticMesh::GetGLMesh() const
    {
        return m_glMesh.get();
    }

    const List<GLStaticVertex>& StaticMesh::GetVertices() const
    {
        return m_vertices;
    }

    const List<uint32>& StaticMesh::GetIndices() const
    {
        return m_indices;
    }
}