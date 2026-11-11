#include "pch.h"
#include "RTShadingPass.h"
#include "RTBruteForceBus.h"

namespace bdt
{
    RTShadingPass::RTShadingPass() = default;

    RTShadingPass::~RTShadingPass()
    {
        Shutdown();
    }

    RTShadingPassUPtr RTShadingPass::Create()
    {
        RTShadingPassUPtr result(new RTShadingPass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTShadingPass::Init()
    {
        m_shadingShader = BDT_RESOURCE.LoadComputeShader
        (
            "RT.BruteForce.Shading",
            { { GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_brute_force_shading.comp" } }
        );

        return true;
    }

    void RTShadingPass::Execute(IRenderBus& bus)
    {
        auto& rtBus = static_cast<RTBruteForceBus&>(bus);
        const auto shader = m_shadingShader.Get();
        if (!shader || !shader->IsValid()) return;

        RTRayQueue* queue = rtBus.GetRayQueue();

        // 1. GPU Buffer 및 Output Texture 확인
        auto* rayBuffer = rtBus.GetRayBuffer();
        auto* hitBuffer = rtBus.GetHitBuffer();
        auto* outputTexture = rtBus.GetOutputTexture();
        if (!rayBuffer || !hitBuffer || !outputTexture) return;

        // 2. Ray / Hit SSBO 바인딩
        rayBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Ray));
        hitBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Hit));

        // 3. Geometry SSBO 바인딩
        BDT_INSTANCE.BindStaticVertexBuffer();
        BDT_INSTANCE.BindSkinnedOutputBuffer();
        BDT_INSTANCE.BindIndexBuffer();
        BDT_INSTANCE.BindDescriptorBuffer();

        // 4. Instance SSBO 바인딩
        BDT_INSTANCE.BindInstanceBuffer();
        BDT_INSTANCE.BindMaterialBuffer();

        // 5. Pixel Accumulation SSBO
        queue->BindAccumulationBuffer();

        // 6. Compute Shader 실행
        shader->Bind();

        // 7. Current Bounce의 Indirect Dispatch
        queue->BindDispatchCommandBuffer();
        glDispatchComputeIndirect(0);
        queue->UnbindDispatchCommandBuffer();

        // 8. Accumulation SSBO 기록 완료 후 동기화
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // 9. Cleanup
        ComputeShader::Unbind();
    }

    void RTShadingPass::Shutdown()
    {
        m_shadingShader = {};
    }
}
