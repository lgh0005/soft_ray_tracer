#pragma once
#include "Mesh.h"
#include <Core/GLStaticMesh.h>

namespace bdt
{
    BDT_CLASS_PTR(StaticMesh)

    class StaticMesh final : public Mesh
    {
        BDT_DISABLE_COPY(StaticMesh)
        BDT_DISABLE_MOVE(StaticMesh)
        using Super = Mesh;

    public:
        virtual ~StaticMesh() override;
        static StaticMeshPtr Create
        (
            const List<GLStaticVertex>& vertices, 
            const List<uint32>& indices
        );
        virtual const GLMesh* GetGLMesh() const override;
        virtual uint32 GetTriangleCount() const override;
        virtual Array<uint32, 3> GetTriangle(uint32 index) const override;

        const List<GLStaticVertex>& GetVertices() const;
        const List<uint32>& GetIndices() const;

    private:
        StaticMesh();

        GLStaticMeshUPtr m_glMesh;
        List<GLStaticVertex> m_vertices;
        List<uint32> m_indices;
    };
}