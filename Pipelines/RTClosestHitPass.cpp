#include "pch.h"
#include "RTClosestHitPass.h"
#include "RTBruteForceBus.h"

namespace bdt
{
    RTClosestHitPass::RTClosestHitPass() = default;

    RTClosestHitPass::~RTClosestHitPass()
    {
        Shutdown();
    }

    RTClosestHitPassUPtr RTClosestHitPass::Create()
    {
        RTClosestHitPassUPtr result(new RTClosestHitPass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTClosestHitPass::Init()
    {
        m_closestHitShader = BDT_RESOURCE.LoadComputeShader(
            "RT.BruteForce.ClosestHit",
            { { GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_brute_force_closest_hit.comp" } }
        );

        return true;
    }

    void RTClosestHitPass::Execute(IRenderBus& bus)
    {
        auto& rtBus = static_cast<RTBruteForceBus&>(bus);

        const auto shader = m_closestHitShader.Get();
        if (!shader || !shader->IsValid()) return;

        // 1. Ray Queue
        RTRayQueue* queue = rtBus.GetRayQueue();
        if (!queue) return;

        // 2. Current Ray / Hit / Counter
        queue->BindRayBuffer();
        queue->BindHitBuffer();
        queue->BindRayCounter();

        // 3. Geometry
        BDT_INSTANCE.BindStaticVertexBuffer();
        BDT_INSTANCE.BindSkinnedOutputBuffer();
        BDT_INSTANCE.BindIndexBuffer();
        BDT_INSTANCE.BindDescriptorBuffer();

        // 4. Instance
        BDT_INSTANCE.BindInstanceBuffer();

        // 5. Compute Shader
        shader->Bind();

        // 6. GPU Indirect Dispatch
        queue->BindDispatchCommandBuffer();
        glDispatchComputeIndirect(0);
        queue->UnbindDispatchCommandBuffer();

        // 7. Hit Buffer -> Next Compute Pass
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        ComputeShader::Unbind();
    }

    void RTClosestHitPass::Shutdown()
    {
        m_closestHitShader = {};
    }
}
