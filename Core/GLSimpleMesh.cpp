#include "pch.h"
#include "GLSimpleMesh.h"

namespace bdt
{
    GLSimpleMesh::GLSimpleMesh() = default;
    GLSimpleMesh::~GLSimpleMesh() = default;

    GLSimpleMeshUPtr GLSimpleMesh::Create
    (
        const List<GLSimpleVertex>& vertices, 
        const List<uint32>& indices
    )
    {
        GLSimpleMeshUPtr mesh(new GLSimpleMesh());
        if (!mesh->Init(vertices, indices)) return nullptr;
        return mesh;
    }

    bool GLSimpleMesh::Init
    (
        const List<GLSimpleVertex>& vertices, 
        const List<uint32>& indices
    )
    {
        if (vertices.empty()) return false;
        return Super::Init
        (
            vertices.data(),
            vertices.size() * sizeof(GLSimpleVertex),
            static_cast<uint32>(vertices.size()),
            indices,
            GL_TRIANGLES
        );
    }
}