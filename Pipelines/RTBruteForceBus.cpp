#include "pch.h"
#include "RTBruteForceBus.h"

namespace bdt
{
    RTBruteForceBus::RTBruteForceBus() = default;
    RTBruteForceBus::~RTBruteForceBus() = default;

    RTBruteForceBusUPtr RTBruteForceBus::Create()
    {
        RTBruteForceBusUPtr bus(new RTBruteForceBus());
        if (!bus->Init()) return nullptr;
        return bus;
    }

    bool RTBruteForceBus::Init()
    {
        // UBOs
        m_cameraBuffer = GLBuffer::Create(sizeof(CameraData), nullptr, GL_DYNAMIC_STORAGE_BIT);
		m_frameDataBuffer = GLBuffer::Create(sizeof(RTFrameData), nullptr, GL_DYNAMIC_STORAGE_BIT);
        if (!m_cameraBuffer || !m_frameDataBuffer) return false;

        // Ray Tracing Buffers
        m_rayBuffer = GLStorageBuffer::Create();
        m_hitBuffer = GLStorageBuffer::Create();
        if (!m_rayBuffer || !m_hitBuffer) return false;

        // Shared RenderScreen
        m_screen = RenderScreen::Create(BDT_WINDOW.GetWindowWidth(), BDT_WINDOW.GetWindowHeight());
        if (!m_screen) return false;

        return true;
    }

    bool RTBruteForceBus::BeginFrame(const RenderScene& scene, uint32 width, uint32 height)
    {
        // 1. Render Target Resize
        if (!Resize(width, height)) return false;

        // 2. Current RenderScene
        SetRenderScene(&scene);

        // 3. Camera UBO Update
        CameraData camera = scene.GetCamera();
        camera.viewportSize = vec2(width, height);
        camera.inverseViewProjection = glm::inverse(camera.viewProjection);
        m_cameraBuffer->SetData(0, sizeof(CameraData), &camera);
        m_cameraBuffer->BindBase(GL_UNIFORM_BUFFER, static_cast<uint32>(EUniformBinding::Camera));

        // 4. Prepare Geometry / Instances
        if (!BDT_INSTANCE.Prepare(scene)) return false;

        // 5. RTFrame UBO
        const uint64 rayCount64 =static_cast<uint64>(width) * height;
        if (rayCount64 > MaxValue<uint32>) return false;   
        RTFrameData frameData{};
        frameData.rayCount = static_cast<uint32>(rayCount64);
        frameData.instanceCount = BDT_INSTANCE.GetInstanceCount();
        m_frameDataBuffer->SetData(0, sizeof(RTFrameData), &frameData);
        m_frameDataBuffer->BindBase(GL_UNIFORM_BUFFER, static_cast<uint32>(ERTUniformBinding::RTFrame));

        // 6. Ray / Hit Buffer Binding
        m_rayBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Ray));
        m_hitBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Hit));

        return true;
    }

    bool RTBruteForceBus::Resize(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0) return false;

        // Skip if dimensions are unchanged
        if (GetWidth() == width &&
            GetHeight() == height &&
            m_outputTexture &&
            m_outputTexture->GetWidth() == width &&
            m_outputTexture->GetHeight() == height &&
            m_screen &&
            m_screen->GetWidth() == width &&
            m_screen->GetHeight() == height) return true;

        // Ray / Hit Buffer Resize
        const uint64 rayCount = static_cast<uint64>(width) * height;
        const uint64 rayBytes = rayCount * sizeof(GPURayWorkItem);
        const uint64 hitBytes = rayCount * sizeof(GPUHit);
        if (rayBytes > MaxValue<uint32> || hitBytes > MaxValue<uint32>) return false;
        if (!m_rayBuffer->Resize(static_cast<uint32>(rayBytes))) return false;
        if (!m_hitBuffer->Resize(static_cast<uint32>(hitBytes))) return false;

        // Output Texture
        RenderTexture2DUPtr newTexture;

        if (!m_outputTexture ||
            m_outputTexture->GetWidth() != width ||
            m_outputTexture->GetHeight() != height)
        {
            newTexture = RenderTexture2D::Create(width, height, GL_RGBA16F);
            if (!newTexture || !newTexture->IsValid()) return false;
        }

        // Shared RenderScreen Resize
        if (!m_screen || !m_screen->Resize(width, height)) return false;
        if (newTexture) m_outputTexture = std::move(newTexture);
        SetSize(width, height);

        return true;
    }

    void RTBruteForceBus::Shutdown()
    {
        // Camera
        m_cameraBuffer.reset();
        m_frameDataBuffer.reset();

        // Ray Tracing
        m_rayBuffer.reset();
        m_hitBuffer.reset();

        // Output
        m_outputTexture.reset();
        m_screen.reset();

        SetRenderScene(nullptr);
        SetSize(0, 0);
    }

    GLStorageBuffer* RTBruteForceBus::GetRayBuffer() const
    {
        return m_rayBuffer.get();
    }

    GLStorageBuffer* RTBruteForceBus::GetHitBuffer() const
    {
        return m_hitBuffer.get();
    }

    RenderTexture2D* RTBruteForceBus::GetOutputTexture() const
    {
        return m_outputTexture.get();
    }

    RenderScreen* RTBruteForceBus::GetRenderScreen() const
    {
        return m_screen.get();
    }
}
