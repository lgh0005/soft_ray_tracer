#include "pch.h"
#include "GLSkinnedMesh.h"

namespace bdt
{
    GLSkinnedMesh::GLSkinnedMesh() = default;
    GLSkinnedMesh::~GLSkinnedMesh() = default;

    GLSkinnedMeshUPtr GLSkinnedMesh::Create
    (
        const List<GLSkinnedVertex>& vertices, 
        const List<uint32>& indices
    )
    {
        GLSkinnedMeshUPtr mesh(new GLSkinnedMesh());
        if (!mesh->Init(vertices, indices)) return nullptr;
        return mesh;
    }

    bool GLSkinnedMesh::Init
    (
        const List<GLSkinnedVertex>& vertices, 
        const List<uint32>& indices
    )
    {
        if (vertices.empty()) return false;
        return GLMesh::Init
        (
            vertices.data(),
            vertices.size() * sizeof(GLSkinnedVertex),
            static_cast<uint32>(vertices.size()),
            indices,
            GL_TRIANGLES
        );
    }
}