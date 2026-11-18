#include "pch.h"
#include "RTBVHDenoiser.h"
#include "RayTracing.h"
#include "RTData.h"
#include "Enums.h"
#include <Core/GLBuffer.h>
#include <Core/GLStorageBuffer.h>
#include <Rendering/RenderScene.h>

namespace bdt
{
    RTBVHDenoiser::RTBVHDenoiser() = default;

    RTBVHDenoiser::~RTBVHDenoiser()
    {
        Shutdown();
    }

    RTBVHDenoiserUPtr RTBVHDenoiser::Create()
    {
        RTBVHDenoiserUPtr denoiser(new RTBVHDenoiser());
        if (!denoiser->Init()) return nullptr;
        return denoiser;
    }

    bool RTBVHDenoiser::Init()
    {
        return true;
    }

    void RTBVHDenoiser::BeginFrame(uint32 frameIndex)
    {
        if (m_primaryReady) m_primaryIndex ^= 1u;
        m_primaryReady = false;
        m_frameIndex = frameIndex;
    }

    void RTBVHDenoiser::Resize(uint32 width, uint32 height)
    {
        if (width == m_primaryWidth && height == m_primaryHeight) return;
        m_primaryReady = false;
        InvalidatePrimarySurfaceHistory();
    }

    void RTBVHDenoiser::Shutdown()
    {
        for (auto& guide : m_primaryGuides) guide.reset();
        m_previousVertices.reset();
        m_previousInstances.reset();
        m_primaryWidth = m_primaryHeight = m_primaryIndex = 0;
        m_vertexBytes = m_instanceBytes = m_frameIndex = 0;
        m_primaryCaptureFrame = MaxValue<uint32>;
        m_primaryReady = false;
        m_primaryReset = true;
        m_primaryVP = m_previousPrimaryVP = m_primaryProjection = mat4(1.0f);
        m_primaryEye = vec3(0.0f);
        m_blasRevision = 0;
        m_previousLights.clear();
        m_previousInstanceLayouts.clear();
        InvalidatePrimarySurfaceHistory();
    }

    bool RTBVHDenoiser::PreparePrimarySurfaceResources(uint32 width, uint32 height, const RTRenderSettings& settings)
    {
        const uint64 pixels = static_cast<uint64>(width) * height;
        if (pixels == 0 || pixels > MaxValue<uint32> / sizeof(GPUDenoisingGuide)) return false;
        if (m_primaryWidth != width || m_primaryHeight != height)
        {
            GLStorageBufferUPtr guides[2];
            for (auto& guide : guides)
            {
                guide = GLStorageBuffer::Create();
                if (!guide || !guide->Resize(static_cast<uint32>(pixels * sizeof(GPUDenoisingGuide)))) 
                    return false;
            }
            for (uint32 i = 0; i < 2; ++i) m_primaryGuides[i] = std::move(guides[i]);
            m_primaryWidth = width;
            m_primaryHeight = height;
            InvalidatePrimarySurfaceHistory();
        }

        uint64 vertices = 16, instances = 16;
        if (settings.temporal)
        {
            vertices = glm::max<uint64>(16, static_cast<uint64>(BDT_INSTANCE.GetSkinnedVertexCount()) * sizeof(GPUStaticVertex));
            instances = glm::max<uint64>(16, static_cast<uint64>(BDT_INSTANCE.GetInstanceCount()) * sizeof(GPUInstance));
        }
        if (vertices > MaxValue<uint32> || instances > MaxValue<uint32>) return false;
        
        if (!m_previousVertices || m_previousVertices->GetBufferSize() < vertices)
        {
            auto buffer = GLBuffer::Create(static_cast<usize>(vertices), nullptr, GL_DYNAMIC_STORAGE_BIT);
            if (!buffer) return false;
            m_previousVertices = std::move(buffer);
            InvalidatePrimarySurfaceHistory();
        }
        
        if (!m_previousInstances || m_previousInstances->GetBufferSize() < instances)
        {
            auto buffer = GLBuffer::Create(static_cast<usize>(instances), nullptr, GL_DYNAMIC_STORAGE_BIT);
            if (!buffer) return false;
            m_previousInstances = std::move(buffer);
            InvalidatePrimarySurfaceHistory();
        }

        m_vertexBytes = static_cast<uint32>(vertices);
        m_instanceBytes = static_cast<uint32>(instances);
        return true;
    }

    bool RTBVHDenoiser::EqualLight(const LightData& a, const LightData& b)
    {
        return a.position == b.position && a.direction == b.direction && a.color == b.color &&
            a.intensity == b.intensity && a.range == b.range && a.type == b.type &&
            a.innerConeAngle == b.innerConeAngle && a.outerConeAngle == b.outerConeAngle &&
            a.attenuationConstant == b.attenuationConstant && a.attenuationLinear == b.attenuationLinear &&
            a.attenuationQuadratic == b.attenuationQuadratic && a.areaRight == b.areaRight &&
            a.areaUp == b.areaUp && a.areaSize == b.areaSize && a.lightFlags == b.lightFlags;
    }

    void RTBVHDenoiser::DetectPrimarySurfaceChanges(const RenderScene& scene, uint64 blasRevision)
    {
        const auto& camera = scene.GetCamera();
        if (m_frameIndex != m_primaryCaptureFrame + 1u ||
            camera.projection != m_primaryProjection || glm::distance(vec3(camera.position), m_primaryEye) > .5f) InvalidatePrimarySurfaceHistory();
        const uint64 revision = blasRevision;
        if (revision != m_blasRevision) InvalidatePrimarySurfaceHistory();
        m_blasRevision = revision;
        List<uvec4> instanceLayouts;
        for (const auto& instance : BDT_INSTANCE.GetInstances())
            instanceLayouts.emplace_back(instance.meshIndex, instance.materialIndex, instance.skinnedVertexOffset, instance.boneCount);
        if (instanceLayouts != m_previousInstanceLayouts) InvalidatePrimarySurfaceHistory();
        m_previousInstanceLayouts = std::move(instanceLayouts);
        const auto& lights = scene.GetLights();
        if (lights.size() != m_previousLights.size()) InvalidatePrimarySurfaceHistory();
        else for (usize i = 0; i < lights.size(); ++i) if (!EqualLight(lights[i], m_previousLights[i])) InvalidatePrimarySurfaceHistory();
        m_previousLights = lights;

    }

    bool RTBVHDenoiser::SavePrimaryGeometry(const RTRenderSettings& settings)
    {
        if (!settings.temporal) return true;
        BDT_INSTANCE.BindSkinnedOutputBuffer();
        BDT_INSTANCE.BindInstanceBuffer();
        GLint vertices = 0, instances = 0;
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, static_cast<uint32>(ERTStorageBinding::SkinnedOutput), &vertices);
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, static_cast<uint32>(ERTStorageBinding::Instance), &instances);
        if (vertices == 0 || instances == 0) return false;
        glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
        glCopyNamedBufferSubData(vertices, m_previousVertices->GetBufferID(), 0, 0, m_vertexBytes);
        glCopyNamedBufferSubData(instances, m_previousInstances->GetBufferID(), 0, 0, m_instanceBytes);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        return true;
    }

    bool RTBVHDenoiser::IsPrimarySurfaceCaptureRequired(const RTRenderSettings& settings) const
    {
        return settings.denoiseEnabled && !settings.visualize &&
            (settings.temporal || settings.spatialIterations != 0) && !IsPrimarySurfaceReady();
    }

    bool RTBVHDenoiser::PreparePrimarySurfaceCapture(const RenderScene& scene, const RTRenderSettings& settings, uint32 width, uint32 height, uint64 blasRevision)
    {
        if (!PreparePrimarySurfaceResources(width, height, settings)) return false;
        DetectPrimarySurfaceChanges(scene, blasRevision);
        m_primaryReset = !settings.temporal || !m_primaryHistoryValid;
        m_previousPrimaryVP = m_primaryVP;
        return true;
    }

    bool RTBVHDenoiser::CompletePrimarySurfaceCapture(const RenderScene& scene, const RTRenderSettings& settings)
    {
        if (!SavePrimaryGeometry(settings))
        {
            InvalidatePrimarySurfaceHistory();
            return false;
        }
        const auto& camera = scene.GetCamera();
        m_primaryVP = camera.viewProjection;
        m_primaryProjection = camera.projection;
        m_primaryEye = vec3(camera.position);
        m_primaryCaptureFrame = m_frameIndex;
        m_primaryHistoryValid = settings.temporal != 0;
        m_primaryReady = true;
        return true;
    }

    bool RTBVHDenoiser::IsPrimarySurfaceReady() const
    {
        return m_primaryReady && m_primaryCaptureFrame == m_frameIndex;
    }

    void RTBVHDenoiser::InvalidatePrimarySurfaceHistory()
    {
        m_primaryHistoryValid = false;
        ++m_primaryRevision;
    }

    bool RTBVHDenoiser::IsPrimarySurfaceHistoryReset() const { return m_primaryReset; }
    GLStorageBuffer* RTBVHDenoiser::GetPrimarySurfaceGuide() const { return m_primaryGuides[m_primaryIndex].get(); }
    GLStorageBuffer* RTBVHDenoiser::GetPreviousPrimarySurfaceGuide() const { return m_primaryGuides[m_primaryIndex ^ 1u].get(); }
    GLBuffer* RTBVHDenoiser::GetPreviousPrimaryVertices() const { return m_previousVertices.get(); }
    GLBuffer* RTBVHDenoiser::GetPreviousPrimaryInstances() const { return m_previousInstances.get(); }
    const mat4& RTBVHDenoiser::GetPreviousPrimaryViewProjection() const { return m_previousPrimaryVP; }
    uint64 RTBVHDenoiser::GetPrimarySurfaceRevision() const { return m_primaryRevision; }

}
