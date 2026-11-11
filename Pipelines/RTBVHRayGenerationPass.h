#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
    class ComputeShader;
    BDT_CLASS_PTR(RTBVHRayGenerationPass)

    class RTBVHRayGenerationPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHRayGenerationPass)
        BDT_DISABLE_MOVE(RTBVHRayGenerationPass)

    public:
        virtual ~RTBVHRayGenerationPass() override;
        static RTBVHRayGenerationPassUPtr Create();
        virtual void Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBVHRayGenerationPass();
        virtual bool Init() override;

        ResourceHandle<ComputeShader> m_rayGenerationShader;
    };
}
