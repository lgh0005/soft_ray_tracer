#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/Instances.h>
#include "RTUniforms.h"

namespace bdt
{
    class RenderScene;
    BDT_CLASS_PTR(GLBuffer)
    BDT_CLASS_PTR(GLStorageBuffer)
    BDT_CLASS_PTR(RTBVHDenoiser)

    class RTBVHDenoiser
    {
        BDT_DISABLE_COPY(RTBVHDenoiser)
        BDT_DISABLE_MOVE(RTBVHDenoiser)

    public:
        ~RTBVHDenoiser();
        static RTBVHDenoiserUPtr Create();
        void BeginFrame(uint32 frameIndex);
        void Resize(uint32 width, uint32 height);
        void Shutdown();

        bool IsPrimarySurfaceCaptureRequired(const RTRenderSettings& settings) const;
        bool PreparePrimarySurfaceCapture(const RenderScene& scene, const RTRenderSettings& settings, uint32 width, uint32 height, uint64 blasRevision);
        bool CompletePrimarySurfaceCapture(const RenderScene& scene, const RTRenderSettings& settings);
        bool IsPrimarySurfaceReady() const;
        bool IsPrimarySurfaceHistoryReset() const;
        void InvalidatePrimarySurfaceHistory();

    public:
        GLStorageBuffer* GetPrimarySurfaceGuide() const;
        GLStorageBuffer* GetPreviousPrimarySurfaceGuide() const;
        GLBuffer* GetPreviousPrimaryVertices() const;
        GLBuffer* GetPreviousPrimaryInstances() const;
        const mat4& GetPreviousPrimaryViewProjection() const;
        uint64 GetPrimarySurfaceRevision() const;

    private:
        RTBVHDenoiser();
        bool Init();
        bool PreparePrimarySurfaceResources(uint32 width, uint32 height, const RTRenderSettings& settings);
        void DetectPrimarySurfaceChanges(const RenderScene& scene, uint64 blasRevision);
        bool SavePrimaryGeometry(const RTRenderSettings& settings);
        static bool EqualLight(const LightData& a, const LightData& b);

    private:
        GLStorageBufferUPtr m_primaryGuides[2];
        GLBufferUPtr m_previousVertices;
        GLBufferUPtr m_previousInstances;
        uint32 m_primaryWidth{ 0 };
        uint32 m_primaryHeight{ 0 };
        uint32 m_primaryIndex{ 0 };
        uint32 m_vertexBytes{ 0 };
        uint32 m_instanceBytes{ 0 };
        uint32 m_primaryCaptureFrame{ MaxValue<uint32> };
        bool m_primaryReady{ false };
        bool m_primaryHistoryValid{ false };
        bool m_primaryReset{ true };
        uint64 m_primaryRevision{ 0 };
        mat4 m_primaryVP{ 1.0f };
        mat4 m_previousPrimaryVP{ 1.0f };
        mat4 m_primaryProjection{ 1.0f };
        vec3 m_primaryEye{ 0.0f };
        uint64 m_blasRevision{ 0 };
        List<LightData> m_previousLights;
        List<uvec4> m_previousInstanceLayouts;

        uint32 m_frameIndex{ 0 };
    };
}
