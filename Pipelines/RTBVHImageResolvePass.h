#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
    class ComputeShader;
    BDT_CLASS_PTR(RTBVHImageResolvePass)

    class RTBVHImageResolvePass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHImageResolvePass)
        BDT_DISABLE_MOVE(RTBVHImageResolvePass)

    public:
        virtual ~RTBVHImageResolvePass() override;
        static RTBVHImageResolvePassUPtr Create();
        virtual void Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBVHImageResolvePass();
        virtual bool Init() override;

        ResourceHandle<ComputeShader> m_resolveShader;
    };
}
