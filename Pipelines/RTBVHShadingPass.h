#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
    class ComputeShader;
    BDT_CLASS_PTR(RTBVHShadingPass)

    class RTBVHShadingPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHShadingPass)
        BDT_DISABLE_MOVE(RTBVHShadingPass)

    public:
        virtual ~RTBVHShadingPass() override;
        static RTBVHShadingPassUPtr Create();
        virtual void Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBVHShadingPass();
        virtual bool Init() override;

        ResourceHandle<ComputeShader> m_shadingShader;
    };
}
