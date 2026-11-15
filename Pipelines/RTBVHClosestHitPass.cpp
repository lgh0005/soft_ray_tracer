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

    bool RTBVHClosestHitPass::Execute(IRenderBus& bus)
    {
        auto& rtBus = static_cast<RTBVHBus&>(bus);

        RTRayQueue* queue = rtBus.GetRayQueue();
        RTBVHStorage* bvh = rtBus.GetBVHStorage();
        if (!queue || !bvh) return false;

        const auto shader = m_closestHitShader.Get();
        if (!shader || !shader->IsValid()) return false;

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

        // 5. Indirect Dispatch
        queue->BindDispatchCommandBuffer();
        glDispatchComputeIndirect(0);
        queue->UnbindDispatchCommandBuffer();

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        ComputeShader::Unbind();
        return true;
    }

    void RTBVHClosestHitPass::Shutdown()
    {

    }
}
