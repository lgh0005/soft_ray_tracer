#pragma once
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>
#include <RayTracing/RTUniforms.h>

namespace bdt
{
    class ComputeShader;
    class RTBVHBus;
    BDT_CLASS_PTR(RenderTexture2D)
    BDT_CLASS_PTR(RTBVHDenoisingPass)

    class RTBVHDenoisingPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHDenoisingPass)
        BDT_DISABLE_MOVE(RTBVHDenoisingPass)

    public:
        ~RTBVHDenoisingPass() override;
        static RTBVHDenoisingPassUPtr Create();
        void InvalidateHistory();
        bool Execute(IRenderBus& bus) override;
        void Shutdown() override;

    private:
        RTBVHDenoisingPass();
        bool Init() override;
        bool PrepareShaders(const RTRenderSettings& settings);
        bool PrepareResources(RTBVHBus& bus);
        void Dispatch(ComputeShader& shader);
        ResourceHandle<ComputeShader> m_temporalShader;
        ResourceHandle<ComputeShader> m_spatialShader;
        RenderTexture2DUPtr m_history[2];
        RenderTexture2DUPtr m_filter[2];
        uint32 m_width{ 0 };
        uint32 m_height{ 0 };
        uint32 m_index{ 0 };
        bool m_historyValid{ false };
        bool m_reset{ true };
        uint64 m_primaryRevision{ 0 };
        uint32 m_lastFrame{ MaxValue<uint32> };
        const RTBVHBus* m_lastBus{ nullptr };

        uint32 m_temporalProgram{ 0 };
        uint32 m_spatialProgram{ 0 };
        int32 m_previousVPLocation{ -1 };
        int32 m_temporalResetLocation{ -1 };
        int32 m_progressiveLocation{ -1 };
        int32 m_historyLimitLocation{ -1 };
        int32 m_stepLocation{ -1 };
    };
}
