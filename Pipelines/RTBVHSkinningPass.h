#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>

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
        virtual void Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBVHSkinningPass();
        virtual bool Init() override;

        ResourceHandle<ComputeShader> m_skinningShader;
    };
}
