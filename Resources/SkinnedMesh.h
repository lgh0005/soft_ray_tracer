#pragma once
#include "Mesh.h"
#include <Core/GLSkinnedMesh.h>

namespace bdt
{
    BDT_CLASS_PTR(SkinnedMesh)

    class SkinnedMesh final : public Mesh
    {
        BDT_DISABLE_COPY(SkinnedMesh)
        BDT_DISABLE_MOVE(SkinnedMesh)
        using Super = Mesh;

    public:
        ~SkinnedMesh() override;
        static SkinnedMeshPtr Create
        (
            const List<GLSkinnedVertex>& vertices, 
            const List<uint32>& indices
        );
        const GLMesh* GetGLMesh() const override;

    public:
        const List<GLSkinnedVertex>& GetVertices() const;
        const List<uint32>& GetIndices() const;

    private:
        SkinnedMesh();

        GLSkinnedMeshUPtr m_glMesh;
        List<GLSkinnedVertex> m_vertices;
        List<uint32> m_indices;
    };
}