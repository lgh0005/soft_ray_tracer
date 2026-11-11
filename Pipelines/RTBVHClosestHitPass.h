#pragma once
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
    class ComputeShader;
    BDT_CLASS_PTR(RTBVHClosestHitPass)

    class RTBVHClosestHitPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHClosestHitPass)
        BDT_DISABLE_MOVE(RTBVHClosestHitPass)

    public:
        virtual ~RTBVHClosestHitPass() override;
        static RTBVHClosestHitPassUPtr Create();
        virtual void Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBVHClosestHitPass();
        virtual bool Init() override;

        ResourceHandle<ComputeShader> m_closestHitShader;
    };
}
