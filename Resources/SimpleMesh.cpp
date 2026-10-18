#include "pch.h"
#include "SimpleMesh.h"

namespace bdt
{
    SimpleMesh::SimpleMesh() = default;
    SimpleMesh::~SimpleMesh() = default;

    SimpleMeshPtr SimpleMesh::Create
    (
        const List<GLSimpleVertex>& vertices, 
        const List<uint32>& indices
    )
    {
        SimpleMeshPtr mesh(new SimpleMesh());

        mesh->m_glMesh = GLSimpleMesh::Create(vertices, indices);
        if (!mesh->m_glMesh) return nullptr;
        mesh->m_vertices = vertices;
        mesh->m_indices = indices;

        return mesh;
    }

    const GLMesh* SimpleMesh::GetGLMesh() const
    {
        return m_glMesh.get();
    }

    const List<GLSimpleVertex>& SimpleMesh::GetVertices() const
    {
        return m_vertices;
    }

    const List<uint32>& SimpleMesh::GetIndices() const
    {
        return m_indices;
    }
}