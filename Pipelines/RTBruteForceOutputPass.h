#pragma once
#include <Rendering/IRenderPass.h>

namespace bdt
{
    BDT_CLASS_PTR(RTBruteForceOutputPass)

        class RTBruteForceOutputPass : public IRenderPass
    {
    public:
        virtual ~RTBruteForceOutputPass() override;
        static RTBruteForceOutputPassUPtr Create();

        virtual void Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBruteForceOutputPass();
        virtual bool Init() override;

        uint32 m_readFramebuffer{ 0 };
    };
}