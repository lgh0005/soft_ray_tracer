#pragma once
#include "GLMesh.h"
#include "GLVertices.h"

namespace bdt
{
    BDT_CLASS_PTR(GLStaticMesh)

    class GLStaticMesh : public GLMesh
    {
        BDT_DISABLE_COPY(GLStaticMesh)
        BDT_DISABLE_MOVE(GLStaticMesh)
        using Super = GLMesh;

    public:
        ~GLStaticMesh() override;
        static GLStaticMeshUPtr Create
        (
            const List<GLStaticVertex>& vertices, 
            const List<uint32>& indices
        );

    private:
        GLStaticMesh();
        bool InitBuffers
        (
            const List<GLStaticVertex>& vertices, 
            const List<uint32>& indices
        );
    };
}