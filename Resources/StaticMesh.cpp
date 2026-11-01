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

	uint32 StaticMesh::GetTriangleCount() const
	{
		return static_cast<uint32>(m_indices.size() / 3);
	}

	Array<uint32, 3> StaticMesh::GetTriangle(uint32 index) const
	{
		Array<uint32, 3> triangle{ 0, 0, 0 };
		if (index >= GetTriangleCount()) return triangle;
		triangle[0] = m_indices[index * 3 + 0];
		triangle[1] = m_indices[index * 3 + 1];
		triangle[2] = m_indices[index * 3 + 2];
		return triangle;
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