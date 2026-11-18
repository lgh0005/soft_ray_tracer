#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>

namespace bdt
{
    BDT_CLASS_PTR(RTBVHPostProcessingPass)

    class RTBVHPostProcessingPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHPostProcessingPass)
        BDT_DISABLE_MOVE(RTBVHPostProcessingPass)

    public:
        virtual ~RTBVHPostProcessingPass() override;
        static RTBVHPostProcessingPassUPtr Create();
        virtual bool Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBVHPostProcessingPass();
        virtual bool Init() override;
    };
}
