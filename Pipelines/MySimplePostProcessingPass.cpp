#include "pch.h"
#include "MySimplePostProcessingPass.h"
#include "MySimpleBus.h"

namespace bdt
{
    MySimplePostProcessingPass::~MySimplePostProcessingPass()
    {
        Shutdown();
    }

    MySimplePostProcessingPassUPtr MySimplePostProcessingPass::Create()
    {
        MySimplePostProcessingPassUPtr postProcess(new MySimplePostProcessingPass());
        if (!postProcess->Init()) return nullptr;
        return postProcess;
    }

    bool MySimplePostProcessingPass::Init()
    {
        // 1. Post-Processing Sampler
        m_sampler = GLSampler::Create();
        if (!m_sampler) return false;
        m_sampler->SetFilter(GL_LINEAR, GL_LINEAR);
        m_sampler->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

        // 2. Post-Processing Shader
        m_toneMappingShader = BDT_RESOURCE.LoadGraphicsShader
        (
            "MySimple.PostProcessing",
            {
                { GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_post_processing.vert" },
                { GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_post_processing.frag" }
            }
        );

        // 3. FXAA Shader
        m_fxaaShader = BDT_RESOURCE.LoadGraphicsShader
        (
            "MySimple.FXAA",
            {
                { GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_post_processing.vert" },
                { GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_fxaa.frag" }
            }
        );

        // 4. Bloom Shader
        m_bloomShader = BDT_RESOURCE.LoadGraphicsShader
        (
            "MySimple.KawaseBloom",
            {
                { GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_post_processing.vert" },
                { GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_kawase_bloom.frag" }
            }
        );

        return true;
    }

    void MySimplePostProcessingPass::Execute(IRenderBus& bus)
    {
        m_bloomReady = false;
        auto* simpleBus = static_cast<MySimpleBus*>(&bus);
        auto* screen = simpleBus->GetRenderScreen();
        auto* ldrFramebuffer = simpleBus->GetLDRFramebuffer();
        auto* ldrTexture = simpleBus->GetLDRTexture();

        auto toneMapping = m_toneMappingShader.Get();
        auto fxaa = m_fxaaShader.Get();
        if (!toneMapping || !fxaa) return;
        if (!toneMapping->IsValid() || !fxaa->IsValid()) return;

        auto* sceneTexture = screen->GetColorTexture();
        if (!sceneTexture || !sceneTexture->IsValid()) return;

        // 2. Default Framebuffer 바인딩
        screen->Unbind();
        glViewport(0, 0, (GLsizei)screen->GetWidth(), (GLsizei)screen->GetHeight());
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_FRAMEBUFFER_SRGB);

        // 3. Bloom
        auto bloom = m_bloomShader.Get();
        if (m_bloomStrength > 0.0f    && 
            bloom && bloom->IsValid() &&
            PrepareBloomTargets(screen->GetWidth(), screen->GetHeight()))
        {
            RenderBloom(sceneTexture, screen, bloom.get());
            m_bloomReady = true;
        }
        glViewport(0, 0, (GLsizei)screen->GetWidth(), (GLsizei)screen->GetHeight());

        // 2. Tone Mapping: HDR -> LDR
        ldrFramebuffer->Bind();
        sceneTexture->Bind(0);
        m_sampler->Bind(0);
        if (m_bloomReady) m_bloomLevels.front().texture->Bind(1);
        else RenderTexture2D::Unbind(1);
        m_sampler->Bind(1);
        toneMapping->Bind();

        // [TODO] 이후 UBO로 통일 고려 필요
        const GLuint program = toneMapping->GetProgramID();
        glProgramUniform1f(program, glGetUniformLocation(program, "uExposure"), m_exposure);
        glProgramUniform1f(program, glGetUniformLocation(program, "uGamma"), m_gamma);
        glProgramUniform1f(program, glGetUniformLocation(program, "uBloomStrength"),
            m_bloomReady ? m_bloomStrength : 0.0f);
        screen->ScreenBlit();

        // 3. FXAA: LDR -> Default Framebuffer
        screen->Unbind();
        ldrTexture->Bind(0);
        fxaa->Bind();
        screen->ScreenBlit();

        // 4. 정리
        GraphicsShader::Unbind();
        glBindTextureUnit(0, 0);
        glBindSampler(0, 0);
        RenderTexture2D::Unbind(1);
        GLSampler::Unbind(1);
        glDepthMask(GL_TRUE);
    }

    void MySimplePostProcessingPass::Shutdown()
    {
        m_toneMappingShader = {};
        m_fxaaShader = {};
        m_bloomShader = {};
        m_bloomLevels.clear();
        m_bloomWidth = m_bloomHeight = 0;
        m_bloomReady = false;
        m_sampler.reset();
    }

    void MySimplePostProcessingPass::SetBloomThreshold(float threshold)
    {
        m_bloomThreshold = glm::max(threshold, 0.0f);
    }

    void MySimplePostProcessingPass::SetBloomStrength(float strength)
    {
        m_bloomStrength = glm::max(strength, 0.0f);
    }

    RenderTexture2D* MySimplePostProcessingPass::GetBloomTexture() const
    {
        return m_bloomReady ? m_bloomLevels.front().texture.get() : nullptr;
    }

    bool MySimplePostProcessingPass::PrepareBloomTargets(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0) return false;
        if (width == m_bloomWidth && height == m_bloomHeight && !m_bloomLevels.empty()) return true;

        List<BloomLevel> levels;
        uint32 w = width, h = height;
        for (uint32 i = 0; i < 5; ++i)
        {
            w = glm::max(w / 2, 1u);
            h = glm::max(h / 2, 1u);
            BloomLevel level;
            level.texture = RenderTexture2D::Create(w, h, GL_R11F_G11F_B10F);
            level.framebuffer = Framebuffer2D::Create(w, h);
            if (!level.texture || !level.framebuffer ||
                !level.framebuffer->AttachColor(0, level.texture->GetTextureID())) return false;
            level.framebuffer->SetDrawBuffers(1);
            if (!level.framebuffer->IsComplete()) return false;
            levels.push_back(std::move(level));
            if (w == 1 && h == 1) break;
        }
        m_bloomLevels = std::move(levels);
        m_bloomWidth = width;
        m_bloomHeight = height;
        return true;
    }

    void MySimplePostProcessingPass::RenderBloom(RenderTexture2D* source, RenderScreen* screen, GraphicsShader* shader)
    {
        shader->Bind();
        m_sampler->Bind(0);

        // [TODO] 이후 UBO로 통일 고려 필요
        const GLuint program = shader->GetProgramID();
        const GLint mode = glGetUniformLocation(program, "uMode");
        glProgramUniform1f(program, glGetUniformLocation(program, "uThreshold"), m_bloomThreshold);

        // Prefilter + downsample: 각 타깃을 덮어써 이전 프레임 누적을 방지한다.
        for (usize i = 0; i < m_bloomLevels.size(); ++i)
        {
            const auto& target = m_bloomLevels[i];
            target.framebuffer->Bind();
            glViewport(0, 0, target.texture->GetWidth(), target.texture->GetHeight());
            (i == 0 ? source : m_bloomLevels[i - 1].texture.get())->Bind(0);
            glProgramUniform1i(program, mode, i == 0 ? 0 : 1);
            screen->ScreenBlit();
        }

        // 작은 단계에서 큰 단계로 Tent 필터를 가산한다.
        GLint srcRGB, dstRGB, srcAlpha, dstAlpha, equationRGB, equationAlpha;
        glGetIntegerv(GL_BLEND_SRC_RGB, &srcRGB);
        glGetIntegerv(GL_BLEND_DST_RGB, &dstRGB);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcAlpha);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &dstAlpha);
        glGetIntegerv(GL_BLEND_EQUATION_RGB, &equationRGB);
        glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &equationAlpha);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        glProgramUniform1i(program, mode, 2);
        for (usize i = m_bloomLevels.size() - 1; i > 0; --i)
        {
            const auto& target = m_bloomLevels[i - 1];
            target.framebuffer->Bind();
            glViewport(0, 0, target.texture->GetWidth(), target.texture->GetHeight());
            m_bloomLevels[i].texture->Bind(0);
            screen->ScreenBlit();
        }
        glDisable(GL_BLEND);
        glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
        glBlendEquationSeparate(equationRGB, equationAlpha);
    }
}
