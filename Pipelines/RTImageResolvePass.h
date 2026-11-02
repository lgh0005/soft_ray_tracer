#pragma once
#include <Rendering/IRenderPass.h>

namespace bdt
{
    BDT_CLASS_PTR(RTImageResolvePass)

    class RTImageResolvePass : public IRenderPass
    {
    public:
        virtual ~RTImageResolvePass() override;
        static RTImageResolvePassUPtr Create();
        virtual void Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTImageResolvePass();
        virtual bool Init() override;

        ResourceHandle<ComputeShader> m_resolveShader;
    };
}
