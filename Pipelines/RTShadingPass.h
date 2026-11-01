#pragma once
#include <Rendering/IRenderPass.h>

namespace bdt
{
    BDT_CLASS_PTR(ComputeShader)
    BDT_CLASS_PTR(RTShadingPass)

    class RTShadingPass : public IRenderPass
    {
    public:
        virtual ~RTShadingPass() override;
        static RTShadingPassUPtr Create();

        virtual void Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTShadingPass();
        virtual bool Init() override;

        ResourceHandle<ComputeShader> m_shadingShader;
    };
}
