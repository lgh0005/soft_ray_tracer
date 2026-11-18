#include "pch.h"
#include "RTBVHSurfacePreparePass.h"
#include "RTBVHBus.h"
#include <RayTracing/RTBVHDenoiser.h>
#include <RayTracing/RTBVHStorage.h>

namespace bdt
{
    RTBVHSurfacePreparePass::RTBVHSurfacePreparePass() = default;

    RTBVHSurfacePreparePass::~RTBVHSurfacePreparePass()
    {
        Shutdown();
    }

    RTBVHSurfacePreparePassUPtr RTBVHSurfacePreparePass::Create()
    {
        RTBVHSurfacePreparePassUPtr result(new RTBVHSurfacePreparePass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTBVHSurfacePreparePass::Init()
    {
        m_shader = BDT_RESOURCE.LoadComputeShader
        (
            "RT.BVH.SurfacePrepare",
            {{
                GL_COMPUTE_SHADER,
                "Assets/Default/Shaders/bdt_rt_bvh_surface_prepare.comp"
            }}
        );

        m_guideShader = BDT_RESOURCE.LoadComputeShader("RT.BVH.Denoising.Guide",
            {{GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_bvh_denoising_guide.comp"}});
        return true;
    }

    bool RTBVHSurfacePreparePass::Execute(IRenderBus& bus)
    {
        auto& rtBus = static_cast<RTBVHBus&>(bus);

        const auto shader = m_shader.Get();
        if (!shader || !shader->IsValid()) return false;

        RTRayQueue* queue = rtBus.GetRayQueue();
        if (!queue || !queue->GetSurfaceBuffer()) return false;

        // 1. Current Ray / Hit
        queue->BindRayBuffer();
        queue->BindHitBuffer();
        queue->BindRayCounter();

        // 2. Geometry
        BDT_INSTANCE.BindStaticVertexBuffer();
        BDT_INSTANCE.BindSkinnedOutputBuffer();
        BDT_INSTANCE.BindIndexBuffer();
        BDT_INSTANCE.BindDescriptorBuffer();

        // 3. Instance / Material
        BDT_INSTANCE.BindInstanceBuffer();
        BDT_INSTANCE.BindMaterialBuffer();

        // 4. Surface Output
        queue->BindSurfaceBuffer();

        // 5. Shader Dispatch
        shader->Bind();
        queue->BindDispatchCommandBuffer();
        glDispatchComputeIndirect(0);
        queue->UnbindDispatchCommandBuffer();

        // Surface SSBO -> Lighting / AO / Shading
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        ComputeShader::Unbind();

        if (rtBus.GetDenoiser()->IsPrimarySurfaceCaptureRequired(rtBus.GetRenderSettings())) return CapturePrimarySurface(rtBus);
        return true;
    }

    bool RTBVHSurfacePreparePass::CapturePrimarySurface(RTBVHBus& bus)
    {
        auto* denoiser = bus.GetDenoiser();
        if (!denoiser || !bus.GetRenderScene() || !bus.GetBVHStorage()) return false;
        const auto shader = m_guideShader.Get();
        if (!shader || !shader->IsValid()) return false;
        if (m_guideProgram != shader->GetProgramID())
        {
            m_guideProgram = shader->GetProgramID();
            m_guideResetLocation = glGetUniformLocation(m_guideProgram, "resetHistory");
            denoiser->InvalidatePrimarySurfaceHistory();
        }
        if (m_guideResetLocation < 0 || !denoiser->PreparePrimarySurfaceCapture(*bus.GetRenderScene(), bus.GetRenderSettings(), bus.GetWidth(), bus.GetHeight(), bus.GetBVHStorage()->GetBLASRevision())) return false;
        auto* queue = bus.GetRayQueue();
        queue->BindRayBuffer();
        queue->BindHitBuffer();
        queue->BindSurfaceBuffer();
        BDT_INSTANCE.BindStaticVertexBuffer();
        BDT_INSTANCE.BindIndexBuffer();
        BDT_INSTANCE.BindDescriptorBuffer();
        BDT_INSTANCE.BindInstanceBuffer();
        denoiser->GetPreviousPrimaryVertices()->BindBase(GL_SHADER_STORAGE_BUFFER, 7);
        denoiser->GetPreviousPrimaryInstances()->BindBase(GL_SHADER_STORAGE_BUFFER, 8);
        denoiser->GetPrimarySurfaceGuide()->Bind(22);
        shader->Bind();
        glUniform1ui(m_guideResetLocation, denoiser->IsPrimarySurfaceHistoryReset() ? 1u : 0u);
        shader->Dispatch((bus.GetWidth() * bus.GetHeight() + 63u) / 64u, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        ComputeShader::Unbind();
        return denoiser->CompletePrimarySurfaceCapture(*bus.GetRenderScene(), bus.GetRenderSettings());
    }

    void RTBVHSurfacePreparePass::Shutdown()
    {
        m_shader = {};
        m_guideShader = {};
        m_guideProgram = 0;
        m_guideResetLocation = -1;
    }
}
