#include "pch.h"
#include "RTBVHBus.h"
#include <RayTracing/RTBVHBuilder.h>
#include <RayTracing/RTBVHDenoiser.h>
#include <RayTracing/RTRenderState.h>
#include <RayTracing/RTEnvironment.h>

namespace bdt
{
    RTBVHBus::RTBVHBus() = default;
    RTBVHBus::~RTBVHBus()
    {
        Shutdown();
    }

    RTBVHBusUPtr RTBVHBus::Create()
    {
        RTBVHBusUPtr bus(new RTBVHBus());
        if (!bus->Init()) return nullptr;
        return bus;
    }

    bool RTBVHBus::Init()
    {
        // UBOs
        m_cameraBuffer = GLBuffer::Create(sizeof(CameraData), nullptr, GL_DYNAMIC_STORAGE_BIT);
		m_frameDataBuffer = GLBuffer::Create(sizeof(RTFrameData), nullptr, GL_DYNAMIC_STORAGE_BIT);
        m_renderState = RTRenderState::Create();
        m_environment = RTEnvironment::Create();
        m_lightBuffer = GLStorageBuffer::Create();
        if (!m_cameraBuffer || !m_frameDataBuffer || !m_renderState || !m_environment || !m_lightBuffer) return false;

        //[수정] Approved full-resolution IBL preset; common settings retain their defaults.
        RTRenderSettings settings = m_renderState->GetRenderSettings();
        settings.maxAreaSampleCount = 4;
        settings.denoiseEnabled = 1;
        settings.aoEnabled = 1;
        settings.environmentEnabled = 1;
        settings.iblEnabled = 1;
        settings.iblSampleCount = 1;
        m_renderState->SetRenderSettings(settings);

        // Ray Tracing Buffers
        m_denoiser = RTBVHDenoiser::Create();
        m_rayQueue = RTRayQueue::Create();
        m_bvhBuilder = RTBVHBuilder::Create(EBVHBuildMode::SAH);
        if (!m_denoiser || !m_rayQueue || !m_bvhBuilder) return false;

        RTBVHBuildSettings buildSettings;
        buildSettings.blasMaxLeafPrimitives = 2;
        buildSettings.tlasMaxLeafPrimitives = 4;
        if (!m_bvhBuilder->SetBuildSettings(buildSettings)) return false;

        // Shared RenderScreen
        m_screen = RenderScreen::Create(BDT_WINDOW.GetWindowWidth(), BDT_WINDOW.GetWindowHeight());
        if (!m_screen) return false;

        return true;
    }

    bool RTBVHBus::BeginFrame(const RenderScene& scene, uint32 width, uint32 height)
    {
        m_denoisedHDRReady = false;
        if (!m_denoiser || !m_renderState) return false;
        m_denoiser->BeginFrame(m_renderState->GetFrameIndex() + 1u);
        // 1. Render Target Resize
        if (!Resize(width, height)) return false;
        width = GetWidth();
        height = GetHeight();

        // 2. Current RenderScene
        SetRenderScene(&scene);

        // 3. Camera UBO Update
        CameraData camera = scene.GetCamera();
        camera.viewportSize = vec2(width, height);
        camera.inverseViewProjection = glm::inverse(camera.viewProjection);
        m_cameraBuffer->SetData(0, sizeof(CameraData), &camera);
        m_cameraBuffer->BindBase(GL_UNIFORM_BUFFER, static_cast<uint32>(EUniformBinding::Camera));

        // Render settings UBO is shared by all BVH passes for this frame.
        if (m_renderState->IsSettingsDirty()) m_denoiser->InvalidatePrimarySurfaceHistory();
        if (!m_renderState->Upload()) return false;
        m_renderState->Bind();
        bool environmentChanged = false;
        if (!m_environment->Prepare(scene.GetImageBasedLight(), GetRenderSettings(), environmentChanged)) return false;
        if (environmentChanged) m_denoiser->InvalidatePrimarySurfaceHistory();

        // 4. Prepare Geometry / Instances
        if (!BDT_INSTANCE.Prepare(scene))
            return false;

        // 5. Builder의 UpdateStorage에서 변경 감지 및 Rebuild/Refit 요청 준비
		if (!m_bvhBuilder->Prepare()) return  false;
        if (!BDT_INSTANCE.Upload()) return false;

        // 5. RTFrame UBO
        const uint64 rayCount64 =static_cast<uint64>(width) * height;
        if (rayCount64 > MaxValue<uint32>) return false;  
        const uint32 rayCount = static_cast<uint32>(rayCount64);
        RTFrameData frameData{};
        frameData.rayCount = rayCount;
        frameData.instanceCount = BDT_INSTANCE.GetInstanceCount();
        frameData.tlasDescriptorIndex = m_bvhBuilder->GetStorage()->GetTLASDescriptorIndex();
        m_frameDataBuffer->SetData(0, sizeof(RTFrameData), &frameData);
        m_frameDataBuffer->BindBase(GL_UNIFORM_BUFFER, static_cast<uint32>(ERTUniformBinding::RTFrame));

        // 6. Ray / Hit Buffer Binding
        m_rayQueue->Clear();
        if (!m_rayQueue->SetInitialRayCount(rayCount)) return false;
        if (!m_rayQueue->SetInitialDispatch(rayCount)) return false;

        // 7. Ray / Hit Buffers
        m_rayQueue->BindRayBuffer();
        m_rayQueue->BindHitBuffer();

        // 8. Ray Queue Counters
        m_rayQueue->BindRayCounter();
        m_rayQueue->BindNextRayCounter();

        // 9. 조명 SSBO 업로드 및 실제 데이터 범위 바인딩
        const auto& lights = scene.GetLights();
        const uint64 lightBytes = static_cast<uint64>(lights.size()) * sizeof(LightData);
        if (lightBytes > MaxValue<uint32>) return false;
        if (!m_lightBuffer->Update(lights.data(), static_cast<uint32>(lightBytes))) return false;
        m_lightBuffer->BindRange(static_cast<uint32>(EStorageBinding::Light));

        m_renderState->AdvanceFrame();
        return true;
    }

    void RTBVHBus::SetRenderSettings(const RTRenderSettings& settings)
    {
        m_renderState->SetRenderSettings(settings);
    }

    const RTRenderSettings& RTBVHBus::GetRenderSettings() const
    {
        return m_renderState->GetRenderSettings();
    }

    uint32 RTBVHBus::GetFrameIndex() const
    {
        if (!m_renderState) return 0;
        return m_renderState->GetFrameIndex();
    }

    RTRenderState* RTBVHBus::GetRenderState() const
    {
        return m_renderState.get();
    }

    bool RTBVHBus::Resize(uint32 width, uint32 height)
    {
        m_denoisedHDRReady = false;
        if (width == 0 || height == 0 || !m_renderState || !m_rayQueue || !m_denoiser) return false;

        width = m_renderState->GetTraceDimension(width);
        height = m_renderState->GetTraceDimension(height);
        if (width == 0 || height == 0 || GetRenderSettings().maxBounces > 3) return false;

        //[수정] Reserve both reflection and refraction branches for the configured bounce count.
        const uint64 maxRayMultiplier = uint64{ 1 } << m_renderState->GetRenderSettings().maxBounces;
        const uint64 pixelCount = static_cast<uint64>(width) * height;
        if (pixelCount > MaxValue<uint32>) return false;
        const uint64 maxRayCount = pixelCount * maxRayMultiplier;
        if (maxRayCount > MaxValue<uint32>) return false;

        // 1. Ray Queue Capacity    
        if (!m_rayQueue->Resize(static_cast<uint32>(maxRayCount), static_cast<uint32>(pixelCount)))
            return false;

        // Skip if dimensions are unchanged
        if (GetWidth() == width &&
            GetHeight() == height &&
            m_hdrTexture &&
            m_hdrTexture->GetWidth() == width &&
            m_hdrTexture->GetHeight() == height &&
            m_outputTexture &&
            m_outputTexture->GetWidth() == width &&
            m_outputTexture->GetHeight() == height &&
            m_screen &&
            m_screen->GetWidth() == width &&
            m_screen->GetHeight() == height) return true;

        // Output Texture
        RenderTexture2DUPtr newTexture;
        RenderTexture2DUPtr newHDRTexture;
        if (!m_hdrTexture || m_hdrTexture->GetWidth() != width || m_hdrTexture->GetHeight() != height)
        {
            newHDRTexture = RenderTexture2D::Create(width, height, GL_RGBA32F);
            if (!newHDRTexture || !newHDRTexture->IsValid()) return false;
        }

        if (!m_outputTexture ||
            m_outputTexture->GetWidth() != width ||
            m_outputTexture->GetHeight() != height)
        {
            newTexture = RenderTexture2D::Create(width, height, GL_RGBA16F);
            if (!newTexture || !newTexture->IsValid()) return false;
        }

        // Shared RenderScreen Resize
        if (!m_screen || !m_screen->Resize(width, height)) return false;
        if (newHDRTexture) m_hdrTexture = std::move(newHDRTexture);
        if (newTexture) m_outputTexture = std::move(newTexture);
        m_denoisedHDRTexture.reset();
        m_denoiser->Resize(width, height);
        SetSize(width, height);

        return true;
    }

    void RTBVHBus::Shutdown()
    {
        m_denoiser.reset();

        // Camera
        m_cameraBuffer.reset();
        m_frameDataBuffer.reset();
        m_renderState.reset();
        m_environment.reset();
        m_lightBuffer.reset();

        // Ray Tracing
        m_rayQueue.reset();
        m_bvhBuilder.reset();

        // Output
        m_denoisedHDRTexture.reset();
        m_denoisedHDRReady = false;
        m_hdrTexture.reset();
        m_outputTexture.reset();
        m_screen.reset();

        SetRenderScene(nullptr);
        SetSize(0, 0);
    }

    GLStorageBuffer* RTBVHBus::GetRayBuffer() const
    {
        return m_rayQueue->GetRayBuffer();
    }

    GLStorageBuffer* RTBVHBus::GetHitBuffer() const
    {
        return m_rayQueue->GetHitBuffer();
    }

    RTBVHStorage* RTBVHBus::GetBVHStorage() const
    {
        return m_bvhBuilder->GetStorage();
    }

    RTBVHBuilder* RTBVHBus::GetBVHBuilder() const
    {
        return m_bvhBuilder.get();
    }

    RTRayQueue* RTBVHBus::GetRayQueue() const
    {
        return m_rayQueue.get();
    }

    RenderTexture2D* RTBVHBus::GetHDRTexture() const
    {
        return m_hdrTexture.get();
    }

    RenderTexture2D* RTBVHBus::GetFinalHDRTexture() const
    {
        if (m_denoisedHDRReady) return m_denoisedHDRTexture.get();
        return m_hdrTexture.get();
    }

    RenderTexture2D* RTBVHBus::GetDenoisedHDRTexture() const
    {
        return m_denoisedHDRTexture.get();
    }

    bool RTBVHBus::PrepareDenoisedHDRTexture()
    {
        if (GetWidth() == 0 || GetHeight() == 0) return false;
        if (m_denoisedHDRTexture) return true;
        m_denoisedHDRTexture = RenderTexture2D::Create(GetWidth(), GetHeight(), GL_RGBA32F);
        return m_denoisedHDRTexture && m_denoisedHDRTexture->IsValid();
    }

    bool RTBVHBus::SetDenoisedHDRReady(bool ready)
    {
        if (ready && (!m_denoisedHDRTexture || !m_denoisedHDRTexture->IsValid())) return false;
        m_denoisedHDRReady = ready;
        return true;
    }

    RenderTexture2D* RTBVHBus::GetOutputTexture() const
    {
        return m_outputTexture.get();
    }

    RenderScreen* RTBVHBus::GetRenderScreen() const
    {
        return m_screen.get();
    }
    RTBVHDenoiser* RTBVHBus::GetDenoiser() const
    {
        return m_denoiser.get();
    }
    RTEnvironment* RTBVHBus::GetEnvironment() const
    {
        return m_environment.get();
    }
}
