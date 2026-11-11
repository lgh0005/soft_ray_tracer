#pragma once
#include <Rendering/IRenderBus.h>

namespace bdt
{
    BDT_CLASS_PTR(GLBuffer)
    BDT_CLASS_PTR(GLStorageBuffer)
    BDT_CLASS_PTR(RenderScreen)
    BDT_CLASS_PTR(RenderTexture2D)
    BDT_CLASS_PTR(RTRayQueue)
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

        // BVH
        bool PrepareBVH();

    private:
        RTBVHBus();
        bool Init();
        GPUBVHNodeBounds TransformBounds
        (
            const GPUBVHNodeBounds& localBounds,
            const mat4& worldMatrix
        ) const;

    private:
        // Shared Camera Resource
        GLBufferUPtr m_cameraBuffer{ nullptr };
		GLBufferUPtr m_frameDataBuffer{ nullptr };

        // Ray Tracing Buffers
        RTRayQueueUPtr m_rayQueue{ nullptr };

        // BVH Buffers
		RTBVHStorageUPtr m_bvhStorage{ nullptr };

        // Mesh별 정점 및 인덱스 캐시
        List<List<vec3>> m_bvhVertices;
        List<List<uint32>> m_bvhIndices;
        List<GPUBVHNodeBounds> m_bvhMeshLocalBounds;

        // BVH 구축 입력
        List<RTBVHMeshInput> m_bvhMeshInputs;
        List<GPUBVHNodeBounds> m_bvhInstanceBounds;

        // Debug / Final Output
        RenderTexture2DUPtr m_outputTexture;

        // Shared RenderScreen
        RenderScreenUPtr m_screen;
    };
}
