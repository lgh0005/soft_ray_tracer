#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>

namespace bdt
{
    BDT_CLASS_PTR(RTBVHOutputPass)

    class RTBVHOutputPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHOutputPass)
        BDT_DISABLE_MOVE(RTBVHOutputPass)

    public:
        virtual ~RTBVHOutputPass() override;
        static RTBVHOutputPassUPtr Create();
        virtual void Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBVHOutputPass();
        virtual bool Init() override;

        uint32 m_readFramebuffer{ 0 };
    };
}
