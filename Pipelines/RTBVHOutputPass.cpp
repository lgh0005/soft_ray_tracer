#include "pch.h"
#include "RTBVHOutputPass.h"
#include "RTBVHBus.h"

namespace bdt
{
    RTBVHOutputPass::RTBVHOutputPass() = default;

    RTBVHOutputPass::~RTBVHOutputPass()
    {
        Shutdown();
    }

    RTBVHOutputPassUPtr RTBVHOutputPass::Create()
    {
        RTBVHOutputPassUPtr result(new RTBVHOutputPass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTBVHOutputPass::Init()
    {
        m_outputShader = BDT_RESOURCE.LoadComputeShader
        (
            "RT.BVH.OutputTransform",
            {{GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_bvh_output_transform.comp"}}
        );
        glCreateFramebuffers(1, &m_readFramebuffer);
        if (m_readFramebuffer == 0) return false;
        return true;
    }

    bool RTBVHOutputPass::Execute(IRenderBus& bus)
    {
        auto& rtBus = static_cast<RTBVHBus&>(bus);

        const auto shader = m_outputShader.Get();
        if (!shader || !shader->IsValid()) return false;
        auto* hdrTexture = rtBus.GetFinalHDRTexture();
        if (!hdrTexture || !hdrTexture->IsValid()) return false;

        // 1. Render Target 확인
        auto* outputTexture = rtBus.GetOutputTexture();
        auto* screen = rtBus.GetRenderScreen();

        if (!outputTexture || !screen)
            return false;

        if (!outputTexture->IsValid() || m_readFramebuffer == 0)
            return false;

        const uint32 width = rtBus.GetWidth();
        const uint32 height = rtBus.GetHeight();

        if (width == 0 || height == 0)
            return false;

        if (hdrTexture->GetWidth() != width || hdrTexture->GetHeight() != height) return false;


        //[수정] Display transform is separate from linear HDR resolve.
        shader->Bind();
        glBindImageTexture(0, hdrTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, outputTexture->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
        shader->Dispatch((width + 7u) / 8u, (height + 7u) / 8u, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_FRAMEBUFFER_BARRIER_BIT);
        glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
        ComputeShader::Unbind();

        // 2. Output Texture를 Read Framebuffer에 연결
        glNamedFramebufferTexture
        (
            m_readFramebuffer,
            GL_COLOR_ATTACHMENT0,
            outputTexture->GetTextureID(),
            0
        );

        glNamedFramebufferReadBuffer
        (
            m_readFramebuffer,
            GL_COLOR_ATTACHMENT0
        );

        // 3. Framebuffer 상태 확인
        if (glCheckNamedFramebufferStatus(m_readFramebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return false;

        // 4. 기본 Framebuffer로 복사
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitNamedFramebuffer
        (
            m_readFramebuffer,
            0,
            0, 0,
            static_cast<GLint>(width),
            static_cast<GLint>(height),
            0, 0,
            static_cast<GLint>(BDT_WINDOW.GetFramebufferWidth()),
            static_cast<GLint>(BDT_WINDOW.GetFramebufferHeight()),
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        return true;
    }

    void RTBVHOutputPass::Shutdown()
    {
        m_outputShader = {};
        if (m_readFramebuffer != 0)
        {
            glDeleteFramebuffers(1, &m_readFramebuffer);
            m_readFramebuffer = 0;
        }
    }
}
