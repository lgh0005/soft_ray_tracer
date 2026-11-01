#include "pch.h"
#include "RTBruteForceOutputPass.h"
#include "RTBruteForceBus.h"

namespace bdt
{
    RTBruteForceOutputPass::RTBruteForceOutputPass() = default;

    RTBruteForceOutputPass::~RTBruteForceOutputPass()
    {
        Shutdown();
    }

    RTBruteForceOutputPassUPtr RTBruteForceOutputPass::Create()
    {
        RTBruteForceOutputPassUPtr result(new RTBruteForceOutputPass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTBruteForceOutputPass::Init()
    {
        // [TEMP]
        glCreateFramebuffers(1, &m_readFramebuffer);
        if (m_readFramebuffer == 0) return false;
        return true;
    }

    void RTBruteForceOutputPass::Execute(IRenderBus& bus)
    {
        auto& rtBus = static_cast<RTBruteForceBus&>(bus);

        // 1. Render Target 확인
        auto* outputTexture = rtBus.GetOutputTexture();
        auto* screen = rtBus.GetRenderScreen();

        if (!outputTexture || !screen)
            return;

        if (!outputTexture->IsValid() || m_readFramebuffer == 0)
            return;

        const uint32 width = rtBus.GetWidth();
        const uint32 height = rtBus.GetHeight();

        if (width == 0 || height == 0)
            return;

        // 2. Output Texture를 Read Framebuffer에 연결
        glNamedFramebufferTexture(
            m_readFramebuffer,
            GL_COLOR_ATTACHMENT0,
            outputTexture->GetTextureID(),
            0
        );

        glNamedFramebufferReadBuffer(
            m_readFramebuffer,
            GL_COLOR_ATTACHMENT0
        );

        // 3. Framebuffer 상태 확인
        if (glCheckNamedFramebufferStatus(
            m_readFramebuffer,
            GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            return;
        }

        // 4. 기본 Framebuffer로 복사
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitNamedFramebuffer(
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
    }

    void RTBruteForceOutputPass::Shutdown()
    {
        if (m_readFramebuffer != 0)
        {
            glDeleteFramebuffers(1, &m_readFramebuffer);
            m_readFramebuffer = 0;
        }
    }
}
