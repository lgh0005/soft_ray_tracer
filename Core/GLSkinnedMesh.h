#pragma once
#include "GLMesh.h"
#include "GLVertices.h"

namespace bdt
{
    BDT_CLASS_PTR(GLSkinnedMesh)

    class GLSkinnedMesh : public GLMesh
    {
        BDT_DISABLE_COPY(GLSkinnedMesh)
        BDT_DISABLE_MOVE(GLSkinnedMesh)
        using Super = GLMesh;

    public:
        ~GLSkinnedMesh() override;
        static GLSkinnedMeshUPtr Create
        (
            const List<GLSkinnedVertex>& vertices, 
            const List<uint32>& indices
        );
        bool InitArray();

    private:
        GLSkinnedMesh();
        bool Init
        (
            const List<GLSkinnedVertex>& vertices,
            const List<uint32>& indices
        );
    };
}