#include "pch.h"
#include "RTBVHImageResolvePass.h"
#include "RTBVHBus.h"

namespace bdt
{
    RTBVHImageResolvePass::RTBVHImageResolvePass() = default;

    RTBVHImageResolvePass::~RTBVHImageResolvePass()
    {
        Shutdown();
    }

    RTBVHImageResolvePassUPtr RTBVHImageResolvePass::Create()
    {
        RTBVHImageResolvePassUPtr result(new RTBVHImageResolvePass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTBVHImageResolvePass::Init()
    {
        m_resolveShader = BDT_RESOURCE.LoadComputeShader
        (
            "RT.BruteForce.ImageResolve",
            { { GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_brute_force_image_resolve.comp" } }
        );
        return true;
    }

    void RTBVHImageResolvePass::Execute(IRenderBus& bus)
    {
        auto& rtBus = static_cast<RTBVHBus&>(bus);

        const auto shader = m_resolveShader.Get();
        if (!shader || !shader->IsValid())
            return;

        const uint32 width = rtBus.GetWidth();
        const uint32 height = rtBus.GetHeight();
        if (width == 0 || height == 0)
            return;

        // 1. Resource 확인
        RTRayQueue* queue = rtBus.GetRayQueue();
        auto* outputTexture = rtBus.GetOutputTexture();
        if (!queue || !outputTexture) return;

        if (!queue->GetAccumulationBuffer())
            return;

        // 2. Accumulation SSBO
        queue->BindAccumulationBuffer();

        // 3. Output Texture
        glBindImageTexture
        (
            0,
            outputTexture->GetTextureID(),
            0,
            GL_FALSE,
            0,
            GL_WRITE_ONLY,
            GL_RGBA16F
        );

        // 4. Compute Shader
        shader->Bind();

        // 5. Pixel 기반 Dispatch
        const uint32 groupX = (width + 7u) / 8u;
        const uint32 groupY = (height + 7u) / 8u;
        shader->Dispatch(groupX, groupY, 1);

        // 6. 이후 Graphics Pass에서 텍스처 읽기
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        // 7. Cleanup
        glBindImageTexture
        (
            0,
            0,
            0,
            GL_FALSE,
            0,
            GL_WRITE_ONLY,
            GL_RGBA16F
        );

        ComputeShader::Unbind();
    }

    void RTBVHImageResolvePass::Shutdown()
    {
        m_resolveShader = {};
    }
}
