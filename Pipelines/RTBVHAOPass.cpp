#include "pch.h"
#include "RTBVHAOPass.h"
#include "RTBVHBus.h"
#include <RayTracing/RTBVHStorage.h>

namespace bdt
{
    RTBVHAOPass::RTBVHAOPass() = default;

    RTBVHAOPass::~RTBVHAOPass()
    {
        Shutdown();
    }

    RTBVHAOPassUPtr RTBVHAOPass::Create()
    {
        RTBVHAOPassUPtr result(new RTBVHAOPass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTBVHAOPass::Init()
    {
        m_shader = BDT_RESOURCE.LoadComputeShader
        (
            "RT.BVH.AO",
            {{GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_bvh_ao.comp"}}
        );
        return true;
    }

    bool RTBVHAOPass::Execute(IRenderBus& bus)
    {
        auto& rtBus = static_cast<RTBVHBus&>(bus);
        auto* queue = rtBus.GetRayQueue();
        if (queue) queue->SetAmbientOcclusionReady(false);
        if (!rtBus.GetRenderSettings().aoEnabled) return true;
        auto* bvh = rtBus.GetBVHStorage();
        const auto shader = m_shader.Get();
        if (!queue || !bvh || !shader || !shader->IsValid()) return false;
        if (bvh->GetTLASDescriptorIndex() == MaxValue<uint32>) return false;
        if (!queue->GetSurfaceBuffer() || !queue->PrepareAmbientOcclusionBuffer()) return false;

        const uint32 program = shader->GetProgramID();
        if (m_shaderProgram != program)
        {
            m_shaderProgram = program;
            m_frameLocation = glGetUniformLocation(program, "uFrameIndex");
        }
        if (m_frameLocation < 0) return false;

        queue->BindRayBuffer();
        queue->BindRayCounter();
        queue->BindSurfaceBuffer();
        queue->BindAmbientOcclusionBuffer();
        BDT_INSTANCE.BindStaticVertexBuffer();
        BDT_INSTANCE.BindSkinnedOutputBuffer();
        BDT_INSTANCE.BindIndexBuffer();
        BDT_INSTANCE.BindDescriptorBuffer();
        BDT_INSTANCE.BindInstanceBuffer();
        bvh->Bind();
        shader->Bind();
        glUniform1ui(m_frameLocation, rtBus.GetFrameIndex());
        queue->BindDispatchCommandBuffer();
        glDispatchComputeIndirect(0);
        queue->UnbindDispatchCommandBuffer();
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        ComputeShader::Unbind();
        return queue->SetAmbientOcclusionReady(true);
    }

    void RTBVHAOPass::Shutdown()
    {
        m_shader = {};
        m_shaderProgram = 0;
        m_frameLocation = -1;
    }
}
