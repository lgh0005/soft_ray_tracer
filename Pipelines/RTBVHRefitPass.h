#pragma once
#include <Rendering/IRenderPass.h>

namespace bdt
{
    BDT_CLASS_PTR(RTBVHRefitPass)

    class RTBVHRefitPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHRefitPass)
        BDT_DISABLE_MOVE(RTBVHRefitPass)

    public:
        virtual ~RTBVHRefitPass() override;
        static RTBVHRefitPassUPtr Create();
        virtual void Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBVHRefitPass();
        virtual bool Init() override;
    };
}
