#include "pch.h"
#include "ScreenMesh.h"
#include <Core/GLScreenMesh.h>

namespace bdt
{
    ScreenMesh::ScreenMesh() = default;
    ScreenMesh::~ScreenMesh() = default;

    ScreenMeshPtr ScreenMesh::Create()
    {
        const List<GLScreenVertex> vertices =
        {
            { vec3(-1.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f) },
            { vec3(1.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f) },
            { vec3(1.0f,  1.0f, 0.0f), vec2(1.0f, 1.0f) },
            { vec3(-1.0f,  1.0f, 0.0f), vec2(0.0f, 1.0f) }
        };

        const List<uint32> indices = { 0, 1, 2, 0, 2, 3 };

        ScreenMeshPtr mesh(new ScreenMesh());
        mesh->m_glMesh = GLScreenMesh::Create(vertices, indices);
        if (!mesh->m_glMesh) return nullptr;

        return mesh;
    }

    const GLMesh* ScreenMesh::GetGLMesh() const
    {
        return m_glMesh.get();
    }

	uint32 ScreenMesh::GetTriangleCount() const
	{
		return 2;
	}

	Array<uint32, 3> ScreenMesh::GetTriangle(uint32 index) const
	{
		if (index >= GetTriangleCount()) return { 0, 0, 0 };
		else if (index == 0) return { 0, 1, 2 };
		else return { 0, 2, 3 };
	}
}