#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
    class ComputeShader;
    BDT_CLASS_PTR(RTBVHSecondaryRayPass)

    class RTBVHSecondaryRayPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHSecondaryRayPass)
        BDT_DISABLE_MOVE(RTBVHSecondaryRayPass)

    public:
        virtual ~RTBVHSecondaryRayPass() override;
        static RTBVHSecondaryRayPassUPtr Create();
        virtual void Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

    private:
        RTBVHSecondaryRayPass();
        virtual bool Init() override;

        ResourceHandle<ComputeShader> m_secondaryRayShader;
        ResourceHandle<ComputeShader> m_prepareDispatchShader;
    };
}
