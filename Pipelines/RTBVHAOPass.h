#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>
#include <RayTracing/RTUniforms.h>

namespace bdt
{
    class ComputeShader;
    BDT_CLASS_PTR(RTBVHAOPass)

    class RTBVHAOPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHAOPass)
        BDT_DISABLE_MOVE(RTBVHAOPass)

    public:
        ~RTBVHAOPass() override;
        static RTBVHAOPassUPtr Create();
        bool Execute(IRenderBus& bus) override;
        void Shutdown() override;

    private:
        RTBVHAOPass();
        bool Init() override;

        ResourceHandle<ComputeShader> m_shader;
        uint32 m_shaderProgram{ 0 };
        int32 m_frameLocation{ -1 };
    };
}
