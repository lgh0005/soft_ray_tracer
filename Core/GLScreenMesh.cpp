#include "pch.h"
#include "GLScreenMesh.h"
#include "GLVertexArray.h"

namespace bdt
{
    GLScreenMesh::GLScreenMesh() = default;
    GLScreenMesh::~GLScreenMesh() = default;

    GLScreenMeshUPtr GLScreenMesh::Create
    (
        const List<GLScreenVertex>& vertices, 
        const List<uint32>& indices
    )
    {
        GLScreenMeshUPtr mesh(new GLScreenMesh());
        if (!mesh->Init(vertices, indices)) return nullptr;
        return mesh;
    }

    bool GLScreenMesh::Init
    (
        const List<GLScreenVertex>& vertices, 
        const List<uint32>& indices
    )
    {
        if (vertices.empty()) return false;
        return Super::Init
        (
            vertices.data(),
            vertices.size() * sizeof(GLStaticVertex),
            static_cast<uint32>(vertices.size()),
            indices,
            GL_TRIANGLES
        );
    }
}