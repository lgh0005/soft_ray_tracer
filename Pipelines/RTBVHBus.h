#pragma once
#include <Rendering/IRenderBus.h>

namespace bdt
{
    BDT_CLASS_PTR(RenderScreen)
    BDT_CLASS_PTR(RenderTexture2D)
    BDT_CLASS_PTR(RTRayQueue)
    BDT_CLASS_PTR(RTBVHBuilder)
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

    public:
        // Ray Tracing Buffers
        GLStorageBuffer* GetRayBuffer() const;
        GLStorageBuffer* GetHitBuffer() const;
        RTBVHStorage* GetBVHStorage() const;
        RTRayQueue* GetRayQueue() const;

        // Output Resources
        RenderTexture2D* GetOutputTexture() const;
        RenderScreen* GetRenderScreen() const;

    private:
        RTBVHBus();
        bool Init();

    private:
        // Shared Camera Resource
        GLBufferUPtr m_cameraBuffer{ nullptr };
		GLBufferUPtr m_frameDataBuffer{ nullptr };

        // Ray Tracing Buffers
        RTRayQueueUPtr m_rayQueue{ nullptr };

        // BVH Builder
        RTBVHBuilderUPtr m_bvhBuilder{ nullptr };

        // Debug / Final Output
        RenderTexture2DUPtr m_outputTexture;

        // Shared RenderScreen
        RenderScreenUPtr m_screen;
    };
}
