#pragma once
#include "Mesh.h"
#include <Core/GLSimpleMesh.h>

namespace bdt
{
    BDT_CLASS_PTR(SimpleMesh)

    class SimpleMesh final : public Mesh
    {
        BDT_DISABLE_COPY(SimpleMesh)
        BDT_DISABLE_MOVE(SimpleMesh)
        using Super = Mesh;

    public:
        ~SimpleMesh() override;
        static SimpleMeshPtr Create
        (
            const List<GLSimpleVertex>& vertices, 
            const List<uint32>& indices
        );
        virtual const GLMesh* GetGLMesh() const override;
        virtual uint32 GetTriangleCount() const override;
        virtual Array<uint32, 3> GetTriangle(uint32 index) const override;

    public:
        const List<GLSimpleVertex>& GetVertices() const;
        const List<uint32>& GetIndices() const;

    private:
        SimpleMesh();

        GLSimpleMeshUPtr m_glMesh;
        List<GLSimpleVertex> m_vertices;
        List<uint32> m_indices;
    };
}