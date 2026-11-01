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

	uint32 SimpleMesh::GetTriangleCount() const
	{
		return static_cast<uint32>(m_indices.size() / 3);
	}

	Array<uint32, 3> SimpleMesh::GetTriangle(uint32 index) const
	{
		Array<uint32, 3> triangle{ 0, 0, 0 };
		if (index >= GetTriangleCount()) return triangle;
		triangle[0] = m_indices[index * 3 + 0];
		triangle[1] = m_indices[index * 3 + 1];
		triangle[2] = m_indices[index * 3 + 2];
		return triangle;
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