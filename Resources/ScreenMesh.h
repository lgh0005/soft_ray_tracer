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
        virtual const GLMesh* GetGLMesh() const override;
        virtual uint32 GetTriangleCount() const override;
        virtual Array<uint32, 3> GetTriangle(uint32 index) const override;

    private:
        ScreenMesh();
        GLScreenMeshUPtr m_glMesh;
    };
}