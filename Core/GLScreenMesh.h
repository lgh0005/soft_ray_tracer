#pragma once
#include "GLMesh.h"
#include "GLVertices.h"

namespace bdt
{
    BDT_CLASS_PTR(GLScreenMesh)

    class GLScreenMesh : public GLMesh
    {
        BDT_DISABLE_COPY(GLScreenMesh)
        BDT_DISABLE_MOVE(GLScreenMesh)
        using Super = GLMesh;

    public:
        ~GLScreenMesh() override;
        static GLScreenMeshUPtr Create
        (
            const List<GLScreenVertex>& vertices, 
            const List<uint32>& indices
        );

    private:
        GLScreenMesh();
        bool Init
        (
            const List<GLScreenVertex>& vertices,
            const List<uint32>& indices
        );
    };
}
