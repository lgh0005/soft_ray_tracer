#pragma once
#include "Mesh.h"

namespace bdt
{
    BDT_CLASS_PTR(GLScreenMesh)
    BDT_CLASS_PTR(ScreenMesh)

    class ScreenMesh final : public Mesh
    {
        BDT_DISABLE_COPY(ScreenMesh)
        BDT_DISABLE_MOVE(ScreenMesh)
        using Super = Mesh;

    public:
        ~ScreenMesh() override;
        static ScreenMeshPtr Create();
        const GLMesh* GetGLMesh() const override;

    private:
        ScreenMesh();
        GLScreenMeshUPtr m_glMesh;
    };
}