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

        const uint32 width = rtBus.GetWidth();
        const uint32 height = rtBus.GetHeight();
        if (width == 0 || height == 0) return;

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

        // 5. Output Image 바인딩
        glBindImageTexture(0, outputTexture->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);

        // 6. Compute Shader 실행
        shader->Bind();

        const uint32 groupX = (width + 7) / 8;
        const uint32 groupY = (height + 7) / 8;

        shader->Dispatch(groupX, groupY, 1);

        // 7. 후속 Graphics Pass에서 Texture 접근 보장
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        // 8. 바인딩 해제
        glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
        ComputeShader::Unbind();
    }

    void RTShadingPass::Shutdown()
    {
        m_shadingShader = {};
    }
}
