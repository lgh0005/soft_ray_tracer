#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>
#include <RayTracing/RTUniforms.h>

namespace bdt
{
    class ComputeShader;
    BDT_CLASS_PTR(RTBVHLightingPass)

    class RTBVHLightingPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHLightingPass)
        BDT_DISABLE_MOVE(RTBVHLightingPass)

    public:
        virtual ~RTBVHLightingPass() override;
        static RTBVHLightingPassUPtr Create();
        virtual bool Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;


    private:
        RTBVHLightingPass();
        virtual bool Init() override;

        ResourceHandle<ComputeShader> m_shader;
        uint32 m_shaderProgramID{ 0 };
        int32 m_frameIndexLocation{ -1 };
    };
}
