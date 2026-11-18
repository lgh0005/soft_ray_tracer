#include "pch.h"
#include "RTRenderState.h"
#include "Enums.h"
#include <Core/GLBuffer.h>

namespace bdt
{
    RTRenderState::RTRenderState() = default;

    RTRenderState::~RTRenderState()
    {
        Shutdown();
    }

    RTRenderStateUPtr RTRenderState::Create()
    {
        RTRenderStateUPtr state(new RTRenderState());
        if (!state->Init()) return nullptr;
        return state;
    }

    bool RTRenderState::Init()
    {
        static_assert(sizeof(RTRenderSettings) == 80);
        m_settingsBuffer = GLBuffer::Create(sizeof(RTRenderSettings), &m_settings, GL_DYNAMIC_STORAGE_BIT);
        return m_settingsBuffer && m_settingsBuffer->IsValid();
    }

    void RTRenderState::SetRenderSettings(const RTRenderSettings& settings)
    {
        m_settings = settings;
        m_settingsDirty = true;
    }

    const RTRenderSettings& RTRenderState::GetRenderSettings() const
    {
        return m_settings;
    }

    uint32 RTRenderState::GetTraceDimension(uint32 dimension) const
    {
        const uint32 divisor = m_settings.traceResolutionDivisor;
        if (divisor == 0) return 0;
        return dimension / divisor + (dimension % divisor != 0 ? 1u : 0u);
    }

    uint32 RTRenderState::GetFrameIndex() const
    {
        return m_frameIndex;
    }

    void RTRenderState::AdvanceFrame()
    {
        ++m_frameIndex;
    }

    bool RTRenderState::IsSettingsDirty() const
    {
        return m_settingsDirty;
    }

    bool RTRenderState::Upload()
    {
        if (!m_settingsBuffer || !m_settingsBuffer->IsValid()) return false;
        if (!m_settingsDirty) return true;
        m_settingsBuffer->SetData(0, sizeof(RTRenderSettings), &m_settings);
        m_settingsDirty = false;
        return true;
    }

    void RTRenderState::Bind() const
    {
        m_settingsBuffer->BindBase(GL_UNIFORM_BUFFER, static_cast<uint32>(ERTUniformBinding::RenderSettings));
    }

    void RTRenderState::Shutdown()
    {
        m_settingsBuffer.reset();
        m_frameIndex = 0;
        m_settings = RTRenderSettings{};
        m_settingsDirty = true;
    }
}
