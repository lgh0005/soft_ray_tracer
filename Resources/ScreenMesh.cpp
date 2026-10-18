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
}