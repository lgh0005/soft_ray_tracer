#pragma once
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
    class ComputeShader;
    BDT_CLASS_PTR(RTBVHRefitPass)

    class RTBVHRefitPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHRefitPass)
        BDT_DISABLE_MOVE(RTBVHRefitPass)

    public:
        virtual ~RTBVHRefitPass() override;
        static RTBVHRefitPassUPtr Create();
        virtual bool Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBVHRefitPass();
        virtual bool Init() override;

    private:
        ResourceHandle<ComputeShader> m_leafRefitShader;
        ResourceHandle<ComputeShader> m_internalRefitShader;
        ResourceHandle<ComputeShader> m_tlasLeafRefitShader;
        ResourceHandle<ComputeShader> m_tlasInternalRefitShader;
    };
}
