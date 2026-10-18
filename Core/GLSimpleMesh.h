#pragma once
#include "GLMesh.h"
#include "GLVertices.h"

namespace bdt
{
    BDT_CLASS_PTR(GLSimpleMesh)

    class GLSimpleMesh : public GLMesh
    {
        BDT_DISABLE_COPY(GLSimpleMesh)
        BDT_DISABLE_MOVE(GLSimpleMesh)
        using Super = GLMesh;

    public:
        ~GLSimpleMesh() override;
        static GLSimpleMeshUPtr Create
        (
            const List<GLSimpleVertex>& vertices, 
            const List<uint32>& indices
        );

    private:
        GLSimpleMesh();
        bool Init
        (
            const List<GLSimpleVertex>& vertices, 
            const List<uint32>& indices
        );
    };
}