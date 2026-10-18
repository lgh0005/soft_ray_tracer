#include "pch.h"
#include "GLStaticMesh.h"

namespace bdt
{
    GLStaticMesh::GLStaticMesh() = default;
    GLStaticMesh::~GLStaticMesh() = default;

    GLStaticMeshUPtr GLStaticMesh::Create
    (
        const List<GLStaticVertex>& vertices, 
        const List<uint32>& indices
    )
    {
        GLStaticMeshUPtr mesh(new GLStaticMesh());
        if (!mesh->InitBuffers(vertices, indices)) return nullptr;
        return mesh;
    }

    bool GLStaticMesh::InitBuffers
    (
        const List<GLStaticVertex>& vertices, 
        const List<uint32>& indices
    )
    {
        if (vertices.empty()) return false;
        return GLMesh::Init
        (
            vertices.data(),
            vertices.size() * sizeof(GLStaticVertex),
            static_cast<uint32>(vertices.size()),
            indices,
            GL_TRIANGLES
        );
    }
}