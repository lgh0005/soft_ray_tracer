#pragma once
#include <Rendering/IRenderBus.h>

namespace bdt
{
    BDT_CLASS_PTR(GLBuffer)
    BDT_CLASS_PTR(GLStorageBuffer)
    BDT_CLASS_PTR(RenderScreen)
    BDT_CLASS_PTR(RTBruteForceBus)

    class RTBruteForceBus : public IRenderBus
    {
        BDT_DISABLE_COPY(RTBruteForceBus)
        BDT_DISABLE_MOVE(RTBruteForceBus)

    public:
        virtual ~RTBruteForceBus() override;
        static RTBruteForceBusUPtr Create();
        bool BeginFrame(const RenderScene& scene, uint32 width, uint32 height);
        virtual bool Resize(uint32 width, uint32 height) override;
        void Shutdown();

    public:
        // Ray Tracing Buffers
        GLStorageBuffer* GetRayBuffer() const;
        GLStorageBuffer* GetHitBuffer() const;

        // Output Resources
        RenderTexture2D* GetOutputTexture() const;
        RenderScreen* GetRenderScreen() const;

    private:
        RTBruteForceBus();
        bool Init();

    private:
        // Shared Camera Resource
        GLBufferUPtr m_cameraBuffer{ nullptr };
		GLBufferUPtr m_frameDataBuffer{ nullptr };

        // Ray Tracing Buffers
        GLStorageBufferUPtr m_rayBuffer{ nullptr };
        GLStorageBufferUPtr m_hitBuffer{ nullptr };

        // Debug / Final Output
        RenderTexture2DUPtr m_outputTexture;

        // Shared RenderScreen
        RenderScreenUPtr m_screen;
    };
}
