#include "pch.h"
#include "RTBVHLightingPass.h"
#include "RTBVHBus.h"

namespace bdt
{
    RTBVHLightingPass::RTBVHLightingPass() = default;

    RTBVHLightingPass::~RTBVHLightingPass()
    {
        Shutdown();
    }

    RTBVHLightingPassUPtr RTBVHLightingPass::Create()
    {
        RTBVHLightingPassUPtr result(new RTBVHLightingPass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTBVHLightingPass::Init()
    {
        m_shader = BDT_RESOURCE.LoadComputeShader
        (
            "RT.BVH.Lighting",
            { { GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_bvh_lighting.comp" } }
        );

        return true;
    }

    bool RTBVHLightingPass::Execute(IRenderBus& bus)
    {
        auto& rtBus = static_cast<RTBVHBus&>(bus);
        const auto shader = m_shader.Get();
        if (!shader || !shader->IsValid()) return false;

        RTRayQueue* queue = rtBus.GetRayQueue();
        RTBVHStorage* bvh = rtBus.GetBVHStorage();
        if (!queue || !bvh || !queue->GetSurfaceBuffer() || !queue->GetDirectLightingBuffer()) return false;
        if (bvh->GetTLASDescriptorIndex() == MaxValue<uint32>) return false;

        const uint32 programID = shader->GetProgramID();
        if (m_shaderProgramID != programID)
        {
            m_frameIndexLocation = glGetUniformLocation(programID, "uFrameIndex");
            m_shaderProgramID = programID;
        }
        if (m_frameIndexLocation < 0) return false;

        queue->BindRayBuffer();
        queue->BindRayCounter();
        queue->BindSurfaceBuffer();
        queue->BindDirectLightingBuffer();

        BDT_INSTANCE.BindMaterialBuffer();
        BDT_INSTANCE.BindStaticVertexBuffer();
        BDT_INSTANCE.BindSkinnedOutputBuffer();
        BDT_INSTANCE.BindIndexBuffer();
        BDT_INSTANCE.BindDescriptorBuffer();
        BDT_INSTANCE.BindInstanceBuffer();
        bvh->Bind();

        shader->Bind();
        glUniform1ui(m_frameIndexLocation, rtBus.GetFrameIndex());
        queue->BindDispatchCommandBuffer();
        glDispatchComputeIndirect(0);
        queue->UnbindDispatchCommandBuffer();

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        ComputeShader::Unbind();
        return true;
    }

    void RTBVHLightingPass::Shutdown()
    {
        m_shader = {};
        m_shaderProgramID = 0;
        m_frameIndexLocation = -1;
    }

}
