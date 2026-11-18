#pragma once
#include <Rendering/IRenderBus.h>
#include <RayTracing/RTUniforms.h>

namespace bdt
{
    class RTBVHStorage;
    BDT_CLASS_PTR(RTBVHDenoiser)
    BDT_CLASS_PTR(RTRenderState)
    BDT_CLASS_PTR(RTEnvironment)
    BDT_CLASS_PTR(GLBuffer)
    BDT_CLASS_PTR(RenderScreen)
    BDT_CLASS_PTR(RenderTexture2D)
    BDT_CLASS_PTR(RTRayQueue)
    BDT_CLASS_PTR(RTBVHBuilder)
    BDT_CLASS_PTR(GLStorageBuffer)
    BDT_CLASS_PTR(RTBVHBus)

    class RTBVHBus : public IRenderBus
    {
        BDT_DISABLE_COPY(RTBVHBus)
        BDT_DISABLE_MOVE(RTBVHBus)

    public:
        virtual ~RTBVHBus() override;
        static RTBVHBusUPtr Create();
        bool BeginFrame(const RenderScene& scene, uint32 width, uint32 height);
        virtual bool Resize(uint32 width, uint32 height) override;
        void Shutdown();

        void SetRenderSettings(const RTRenderSettings& settings);
        const RTRenderSettings& GetRenderSettings() const;

    public:
        // Ray Tracing Buffers
        GLStorageBuffer* GetRayBuffer() const;
        GLStorageBuffer* GetHitBuffer() const;
        RTBVHStorage* GetBVHStorage() const;
        RTBVHBuilder* GetBVHBuilder() const;
        RTRayQueue* GetRayQueue() const;
        uint32 GetFrameIndex() const;
        RTRenderState* GetRenderState() const;
        RTEnvironment* GetEnvironment() const;
        RTBVHDenoiser* GetDenoiser() const;

        // Output Resources
        RenderTexture2D* GetHDRTexture() const;
        RenderTexture2D* GetFinalHDRTexture() const;
        RenderTexture2D* GetDenoisedHDRTexture() const;
        bool PrepareDenoisedHDRTexture();
        bool SetDenoisedHDRReady(bool ready);
        RenderTexture2D* GetOutputTexture() const;
        RenderScreen* GetRenderScreen() const;

    private:
        RTBVHBus();
        bool Init();

    private:

        // Ray Tracing Render State
        RTRenderStateUPtr m_renderState;
        RTEnvironmentUPtr m_environment;

        // Shared Camera Resource
        GLBufferUPtr m_cameraBuffer{ nullptr };
		GLBufferUPtr m_frameDataBuffer{ nullptr };
        GLStorageBufferUPtr m_lightBuffer{ nullptr };

        // Ray Tracing Buffers
        RTRayQueueUPtr m_rayQueue{ nullptr };

        // BVH Builder
        RTBVHBuilderUPtr m_bvhBuilder{ nullptr };

        // Denoiser
        RTBVHDenoiserUPtr m_denoiser;

        // Linear HDR and display output
        RenderTexture2DUPtr m_hdrTexture{ nullptr };
        RenderTexture2DUPtr m_denoisedHDRTexture{ nullptr };
        bool m_denoisedHDRReady{ false };

        // Debug / Final Output
        RenderTexture2DUPtr m_outputTexture{ nullptr };

        // Shared RenderScreen
        RenderScreenUPtr m_screen{ nullptr };
    };
}
