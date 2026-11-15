#pragma once
#include <Rendering/IRenderPass.h>

namespace bdt
{
    class ComputeShader;
    BDT_CLASS_PTR(RTBVHSkinningPass)

    class RTBVHSkinningPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHSkinningPass)
        BDT_DISABLE_MOVE(RTBVHSkinningPass)

    public:
        virtual ~RTBVHSkinningPass() override;
        static RTBVHSkinningPassUPtr Create();
        virtual bool Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBVHSkinningPass();
        virtual bool Init() override;

        ResourceHandle<ComputeShader> m_skinningShader;
    };
}
