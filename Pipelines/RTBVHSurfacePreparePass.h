#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
    class ComputeShader;
    class RTBVHBus;
    BDT_CLASS_PTR(RTBVHSurfacePreparePass)

    class RTBVHSurfacePreparePass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHSurfacePreparePass)
        BDT_DISABLE_MOVE(RTBVHSurfacePreparePass)

    public:
        virtual ~RTBVHSurfacePreparePass() override;
        static RTBVHSurfacePreparePassUPtr Create();
        virtual bool Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBVHSurfacePreparePass();
        virtual bool Init() override;

        bool CapturePrimarySurface(RTBVHBus& bus);
        ResourceHandle<ComputeShader> m_shader;
        ResourceHandle<ComputeShader> m_guideShader;
        uint32 m_guideProgram{ 0 };
        int32 m_guideResetLocation{ -1 };
    };
}
