#include "pch.h"
#include "RTEnvironment.h"
#include "Enums.h"
#include <Core/GLBuffer.h>
#include <Core/GLSampler.h>
#include <Core/GLTexture2D.h>
#include <Rendering/ImageBasedLight.h>
#include <Rendering/RenderTexture2D.h>
#include <Resources/ResourceManager.h>
#include <Resources/Resource.h>

namespace bdt
{
    RTEnvironment::RTEnvironment() = default;
    RTEnvironment::~RTEnvironment() { Shutdown(); }
    RTEnvironmentUPtr RTEnvironment::Create()
    {
        RTEnvironmentUPtr environment(new RTEnvironment());
        if (!environment->Init()) return nullptr;
        return environment;
    }
    bool RTEnvironment::Init()
    {
        static_assert(sizeof(EnvironmentData) == 32);
        m_buffer = GLBuffer::Create(sizeof(EnvironmentData), &m_data, GL_DYNAMIC_STORAGE_BIT);
        m_environmentSampler = GLSampler::Create();
        m_distributionSampler = GLSampler::Create();
        if (!m_buffer || !m_environmentSampler || !m_distributionSampler) return false;
        m_environmentSampler->SetWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
        m_environmentSampler->SetFilter(GL_LINEAR, GL_LINEAR);
        m_distributionSampler->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        m_distributionSampler->SetFilter(GL_NEAREST, GL_NEAREST);
        m_rowsShader = BDT_RESOURCE.LoadComputeShader("RT.Environment.CDF.Rows", {{GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_environment_cdf_rows.comp"}});
        m_marginalShader = BDT_RESOURCE.LoadComputeShader("RT.Environment.CDF.Marginal", {{GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_environment_cdf_marginal.comp"}});
        return true;
    }
    bool RTEnvironment::Prepare(const ImageBasedLight* light, const RTRenderSettings& settings, bool& changed)
    {
        changed = false;
        const EnvironmentData previous = m_data;
        const bool rebuilding = m_distributionDirty;
        Image2DPtr source;
        if (light && settings.environmentEnabled) source = light->GetSource();
        uint32 texture = 0;
        if (source && source->IsValid() && source->GetTexture()->GetTarget() == GL_TEXTURE_2D)
            texture = source->GetTexture()->GetTextureID();
        if (source != m_source || texture != m_sourceTexture)
        {
            m_source = source;
            m_sourceTexture = texture;
            m_distributionDirty = true;
            changed = true;
        }
        const float angle = settings.environmentRotation;
        const bool validAngle = !glm::isnan(angle) && !glm::isinf(angle);
        m_data.transform = vec4(1.0f, 0.0f, 0.0f, 0.0f);
        if (validAngle) m_data.transform = vec4(glm::cos(angle), glm::sin(angle), 0.0f, 0.0f);
        if (texture != 0 && validAngle)
        {
            m_data.transform.z = light->GetIntensity() * settings.iblIntensity;
            if (!glm::isnan(m_data.transform.z) && !glm::isinf(m_data.transform.z)) m_data.transform.w = 1.0f;
            else m_data.transform.z = 0.0f;
            if (m_distributionDirty && !BuildDistribution()) return false;
        }
        changed = changed || (rebuilding && texture != 0) || previous.transform != m_data.transform;
        if (changed) m_buffer->SetData(0, sizeof(EnvironmentData), &m_data);
        Bind();
        return true;
    }
    bool RTEnvironment::BuildDistribution()
    {
        const auto rows = m_rowsShader.Get();
        const auto marginal = m_marginalShader.Get();
        if (!rows || !rows->IsValid() || !marginal || !marginal->IsValid()) return false;
        if (!m_distribution)
        {
            m_distribution = RenderTexture2D::Create(m_data.distribution.x, m_data.distribution.y + 1u, GL_RG32F);
            if (!m_distribution) return false;
        }
        Bind();
        glBindImageTexture(0, m_distribution->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
        rows->Bind();
        rows->Dispatch((m_data.distribution.y + 63u) / 64u, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        marginal->Bind();
        marginal->Dispatch(1, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
        ComputeShader::Unbind();
        m_distributionDirty = false;
        return true;
    }
    void RTEnvironment::Bind() const
    {
        glBindTextureUnit(16, m_sourceTexture);
        m_environmentSampler->Bind(16);
        glBindTextureUnit(17, m_distribution ? m_distribution->GetTextureID() : 0);
        m_distributionSampler->Bind(17);
        m_buffer->BindBase(GL_UNIFORM_BUFFER, static_cast<uint32>(ERTUniformBinding::Environment));
    }
    void RTEnvironment::Invalidate() { m_distributionDirty = true; }
    void RTEnvironment::Shutdown()
    {
        m_buffer.reset();
        m_distribution.reset();
        m_environmentSampler.reset();
        m_distributionSampler.reset();
        m_source.reset();
        m_rowsShader = {};
        m_marginalShader = {};
        m_sourceTexture = 0;
        m_distributionDirty = true;
        m_data = EnvironmentData{};
    }
}
