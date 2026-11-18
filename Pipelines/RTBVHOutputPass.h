#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
    class ComputeShader;
    BDT_CLASS_PTR(RTBVHOutputPass)

    class RTBVHOutputPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHOutputPass)
        BDT_DISABLE_MOVE(RTBVHOutputPass)

    public:
        virtual ~RTBVHOutputPass() override;
        static RTBVHOutputPassUPtr Create();
        virtual bool Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBVHOutputPass();
        virtual bool Init() override;

        ResourceHandle<ComputeShader> m_outputShader;
        uint32 m_readFramebuffer{ 0 };
    };
}
