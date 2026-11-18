#pragma once
#include "RTUniforms.h"

namespace bdt
{
    BDT_CLASS_PTR(GLBuffer)
    BDT_CLASS_PTR(RTRenderState)

    class RTRenderState
    {
        BDT_DISABLE_COPY(RTRenderState)
        BDT_DISABLE_MOVE(RTRenderState)

    public:
        ~RTRenderState();
        static RTRenderStateUPtr Create();
        bool Upload();
        void Bind() const;
        void Shutdown();

    public:
        void SetRenderSettings(const RTRenderSettings& settings);
        const RTRenderSettings& GetRenderSettings() const;
        uint32 GetTraceDimension(uint32 dimension) const;
        uint32 GetFrameIndex() const;
        void AdvanceFrame();
        bool IsSettingsDirty() const;

    private:
        RTRenderState();
        bool Init();

    private:
        uint32 m_frameIndex{ 0 };
        RTRenderSettings m_settings{};
        bool m_settingsDirty{ true };
        GLBufferUPtr m_settingsBuffer{ nullptr };
    };
}
