#include "pch.h"
#include "RTBVHDenoisingPass.h"
#include "RTBVHBus.h"
#include <RayTracing/RTBVHDenoiser.h>
#include <RayTracing/RTData.h>
#include <Rendering/RenderTexture2D.h>
#include <Rendering/RenderObject.h>
#include <Core/GLTexture2D.h>
#include <glm/gtc/type_ptr.hpp>

namespace bdt
{
    RTBVHDenoisingPass::RTBVHDenoisingPass() = default;
    RTBVHDenoisingPass::~RTBVHDenoisingPass()
    {
        Shutdown();
    }

    RTBVHDenoisingPassUPtr RTBVHDenoisingPass::Create()
    {
        RTBVHDenoisingPassUPtr pass(new RTBVHDenoisingPass());
        if (!pass->Init()) return nullptr;
        return pass;
    }

    bool RTBVHDenoisingPass::Init()
    {
        m_temporalShader = BDT_RESOURCE.LoadComputeShader("RT.BVH.Denoising.Temporal",
            {{GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_bvh_denoising_temporal.comp"}});
        m_spatialShader = BDT_RESOURCE.LoadComputeShader("RT.BVH.Denoising.Spatial",
            {{GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_bvh_denoising_spatial.comp"}});
        return true;
    }

    void RTBVHDenoisingPass::InvalidateHistory()
    {
        m_historyValid = false;
    }

    bool RTBVHDenoisingPass::PrepareShaders(const RTRenderSettings& settings)
    {
        if (settings.temporal)
        {
            const auto temporal = m_temporalShader.Get();
            if (!temporal || !temporal->IsValid()) return false;
            if (m_temporalProgram != temporal->GetProgramID())
            {
                m_temporalProgram = temporal->GetProgramID();
                m_previousVPLocation = glGetUniformLocation(m_temporalProgram, "previousVP");
                m_temporalResetLocation = glGetUniformLocation(m_temporalProgram, "resetHistory");
                m_progressiveLocation = glGetUniformLocation(m_temporalProgram, "progressive");
                m_historyLimitLocation = glGetUniformLocation(m_temporalProgram, "historyLimit");
                InvalidateHistory();
            }
            if (m_previousVPLocation < 0 || m_temporalResetLocation < 0 || m_progressiveLocation < 0 || m_historyLimitLocation < 0) return false;
        }
        if (settings.spatialIterations != 0)
        {
            const auto spatial = m_spatialShader.Get();
            if (!spatial || !spatial->IsValid()) return false;
            if (m_spatialProgram != spatial->GetProgramID())
            {
                m_spatialProgram = spatial->GetProgramID();
                m_stepLocation = glGetUniformLocation(m_spatialProgram, "stepWidth");
                InvalidateHistory();
            }
            if (m_stepLocation < 0) return false;
        }
        return true;
    }

    bool RTBVHDenoisingPass::PrepareResources(RTBVHBus& bus)
    {
        static_assert(sizeof(GPUDenoisingGuide) == 64);
        const uint32 width = bus.GetWidth();
        const uint32 height = bus.GetHeight();
        const uint64 pixels = static_cast<uint64>(width) * height;
        if (pixels == 0 || pixels > MaxValue<uint32> / sizeof(GPUDenoisingGuide)) return false;
        if (width != m_width || height != m_height)
        {
            RenderTexture2DUPtr history[2], filter[2];
            for (uint32 i = 0; i < 2; ++i)
            {
                history[i] = RenderTexture2D::Create(width, height, GL_RGBA32F);
                filter[i] = RenderTexture2D::Create(width, height, GL_RGBA32F);
                if (!history[i] || !filter[i]) return false;
            }
            for (uint32 i = 0; i < 2; ++i)
            {
                m_history[i] = std::move(history[i]);
                m_filter[i] = std::move(filter[i]);
            }
            m_width = width;
            m_height = height;
            m_index = 0;
            InvalidateHistory();
        }
        return bus.PrepareDenoisedHDRTexture();
    }

    void RTBVHDenoisingPass::Dispatch(ComputeShader& shader)
    {
        shader.Dispatch((m_width + 7u) / 8u, (m_height + 7u) / 8u, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    bool RTBVHDenoisingPass::Execute(IRenderBus& bus)
    {
        auto& rtBus = static_cast<RTBVHBus&>(bus);
        const auto& settings = rtBus.GetRenderSettings();
        rtBus.SetDenoisedHDRReady(false);
        if (!settings.denoiseEnabled || (!settings.temporal && settings.spatialIterations == 0))
        {
            InvalidateHistory();
            return true;
        }
        const auto* denoiser = rtBus.GetDenoiser();
        if (!denoiser || !denoiser->IsPrimarySurfaceReady()) return false;
        if (!PrepareShaders(settings) || !PrepareResources(rtBus)) return false;
        m_reset = !settings.temporal || !m_historyValid || denoiser->IsPrimarySurfaceHistoryReset() ||
            m_lastBus != &rtBus || rtBus.GetFrameIndex() != m_lastFrame + 1u ||
            m_primaryRevision != denoiser->GetPrimarySurfaceRevision();
        auto* raw = rtBus.GetHDRTexture();
        auto* target = rtBus.GetDenoisedHDRTexture();
        if (!raw || !target || raw->GetWidth() != m_width || raw->GetHeight() != m_height) return false;
        GLuint input = raw->GetTextureID();
        if (settings.temporal)
        {
            const auto shader = m_temporalShader.Get();
            shader->Bind();
            denoiser->GetPrimarySurfaceGuide()->Bind(0);
            denoiser->GetPreviousPrimarySurfaceGuide()->Bind(1);
            glBindImageTexture(0, input, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
            glBindImageTexture(1, m_history[m_index ^ 1u]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
            glBindImageTexture(2, m_history[m_index]->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
            glUniformMatrix4fv(m_previousVPLocation, 1, GL_FALSE, glm::value_ptr(denoiser->GetPreviousPrimaryViewProjection()));
            glUniform1ui(m_temporalResetLocation, m_reset ? 1u : 0u);
            glUniform1ui(m_progressiveLocation, 0u);
            glUniform1ui(m_historyLimitLocation, 8u);
            Dispatch(*shader);
            input = m_history[m_index]->GetTextureID();
        }
        for (uint32 iteration = 0; iteration < settings.spatialIterations; ++iteration)
        {
            GLuint output = target->GetTextureID();
            if (iteration + 1u < settings.spatialIterations) output = m_filter[iteration & 1u]->GetTextureID();
            const auto shader = m_spatialShader.Get();
            shader->Bind();
            denoiser->GetPrimarySurfaceGuide()->Bind(0);
            glBindImageTexture(0, input, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
            glBindImageTexture(1, output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
            glUniform1i(m_stepLocation, 1 << iteration);
            Dispatch(*shader);
            input = output;
        }
        if (input != target->GetTextureID())
        {
            glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
            glCopyImageSubData(input, GL_TEXTURE_2D, 0, 0, 0, 0, target->GetTextureID(), GL_TEXTURE_2D, 0, 0, 0, 0, m_width, m_height, 1);
        }
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        for (uint32 unit = 0; unit < 3; ++unit) glBindImageTexture(unit, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        ComputeShader::Unbind();
        m_primaryRevision = denoiser->GetPrimarySurfaceRevision();
        m_lastBus = &rtBus;
        m_lastFrame = rtBus.GetFrameIndex();
        m_historyValid = settings.temporal;
        m_index ^= 1u;
        return rtBus.SetDenoisedHDRReady(true);
    }

    void RTBVHDenoisingPass::Shutdown()
    {
        InvalidateHistory();
        for (uint32 i = 0; i < 2; ++i)
        {
            m_history[i].reset();
            m_filter[i].reset();
        }
        m_temporalShader = {};
        m_spatialShader = {};
        m_width = m_height = m_index = 0;
        m_temporalProgram = m_spatialProgram = 0;
        m_lastFrame = MaxValue<uint32>;
        m_lastBus = nullptr;
    }
}
