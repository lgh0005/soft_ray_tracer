#pragma once
#include <Rendering/IRenderPass.h>

namespace bdt
{
    BDT_CLASS_PTR(RTSkinningPass)

    class RTSkinningPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTSkinningPass)
        BDT_DISABLE_MOVE(RTSkinningPass)

    public:
        virtual ~RTSkinningPass() override;
        static RTSkinningPassUPtr Create();
        virtual void Execute(IRenderBus& bus) override;
        void Shutdown();

    private:
        RTSkinningPass();
        bool Init();

    private:
        ResourceHandle<ComputeShader> m_skinningShader;
    };
}
