#include "pch.h"
#include "Mesh.h"
#include <Core/GLMesh.h>

namespace bdt
{
    Mesh::Mesh() = default;
    Mesh::~Mesh() = default;

    bool Mesh::IsValid() const
    {
        const GLMesh* mesh = GetGLMesh();
        return mesh && mesh->IsValid();
    }
}