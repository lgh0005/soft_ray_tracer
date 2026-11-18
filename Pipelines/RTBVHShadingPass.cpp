#include "pch.h"
#include "RTBVHShadingPass.h"
#include "RTBVHBus.h"

namespace bdt
{
    RTBVHShadingPass::RTBVHShadingPass() = default;

    RTBVHShadingPass::~RTBVHShadingPass()
    {
        Shutdown();
    }

    RTBVHShadingPassUPtr RTBVHShadingPass::Create()
    {
        RTBVHShadingPassUPtr result(new RTBVHShadingPass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTBVHShadingPass::Init()
    {
        m_shadingShader = BDT_RESOURCE.LoadComputeShader
        (
            "RT.BVH.Shading",
            { { GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_bvh_shading.comp" } }
        );

        return true;
    }

    bool RTBVHShadingPass::Execute(IRenderBus& bus)
    {
        auto& rtBus = static_cast<RTBVHBus&>(bus);
        const auto shader = m_shadingShader.Get();
        if (!shader || !shader->IsValid()) return false;

        RTRayQueue* queue = rtBus.GetRayQueue();
        if (!queue) return false;

        // 1. GPU Buffer 및 Output Texture 확인
        auto* rayBuffer = rtBus.GetRayBuffer();
        auto* hitBuffer = rtBus.GetHitBuffer();
        auto* outputTexture = rtBus.GetOutputTexture();
        if (!rayBuffer || !hitBuffer || !outputTexture) return false;

        // 2. Ray / Hit SSBO 바인딩
        rayBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Ray));
        hitBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Hit));

        // 3. Prepared Surface (written by RTBVHSurfacePreparePass)
        queue->BindSurfaceBuffer();
        queue->BindDirectLightingBuffer();
        queue->BindRayCounter();

        // Material and light evaluation are handled by Lighting Pass.

        // 5. Pixel Accumulation SSBO
        queue->BindAccumulationBuffer();

        if (rtBus.GetRenderSettings().visualize)
        {
            if (!queue->IsAmbientOcclusionReady() || !queue->GetAmbientOcclusionBuffer()) return false;
            queue->BindAmbientOcclusionBuffer();
        }

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
        return true;
    }

    void RTBVHShadingPass::Shutdown()
    {
        m_shadingShader = {};
    }
}
