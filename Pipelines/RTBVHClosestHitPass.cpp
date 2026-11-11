#include "pch.h"
#include "RTBVHClosestHitPass.h"
#include "RTBVHBus.h"

namespace bdt
{
    RTBVHClosestHitPass::RTBVHClosestHitPass() = default;

    RTBVHClosestHitPass::~RTBVHClosestHitPass()
    {
        Shutdown();
    }

    RTBVHClosestHitPassUPtr RTBVHClosestHitPass::Create()
    {
        RTBVHClosestHitPassUPtr result(new RTBVHClosestHitPass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTBVHClosestHitPass::Init()
    {
        m_closestHitShader = BDT_RESOURCE.LoadComputeShader
        (
            "RT.BVH.ClosestHit",
            {{GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_bvh_closest_hit.comp"}}
        );

        return true;
    }

    void RTBVHClosestHitPass::Execute(IRenderBus& bus)
    {
        auto& rtBus = static_cast<RTBVHBus&>(bus);

        RTRayQueue* queue = rtBus.GetRayQueue();
        RTBVHStorage* bvh = rtBus.GetBVHStorage();
        if (!queue || !bvh) return;

        const auto shader = m_closestHitShader.Get();
        if (!shader || !shader->IsValid()) return;

        // 1. Ray / Hit / Counter
        queue->BindRayBuffer();
        queue->BindHitBuffer();
        queue->BindRayCounter();

        // 2. Geometry / Instance
        BDT_INSTANCE.BindStaticVertexBuffer();
        BDT_INSTANCE.BindSkinnedOutputBuffer();
        BDT_INSTANCE.BindIndexBuffer();
        BDT_INSTANCE.BindDescriptorBuffer();
        BDT_INSTANCE.BindInstanceBuffer();

        // 3. BVH
        bvh->Bind();

        // 4. Compute Shader
        shader->Bind();

        // TLAS Descriptor는 모든 BLAS Descriptor 다음에 위치
        glUniform1ui(0, BDT_INSTANCE.GetGeometryCount());

        // 5. Indirect Dispatch
        queue->BindDispatchCommandBuffer();
        glDispatchComputeIndirect(0);
        queue->UnbindDispatchCommandBuffer();

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        ComputeShader::Unbind();
    }

    void RTBVHClosestHitPass::Shutdown()
    {
    }
}
