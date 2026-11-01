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

        const uint32 width = rtBus.GetWidth();
        const uint32 height = rtBus.GetHeight();
        if (width == 0 || height == 0) return;

        // 1. Ray Count 확인
        const uint64 rayCount64 = static_cast<uint64>(width) * height;
        if (rayCount64 > MaxValue<uint32>) return;

        const uint32 rayCount = static_cast<uint32>(rayCount64);

        // 2. Ray / Hit Buffer 확인
        auto* rayBuffer = rtBus.GetRayBuffer();
        auto* hitBuffer = rtBus.GetHitBuffer();
        if (!rayBuffer || !hitBuffer) return;

        // 3. SSBO 바인딩
        rayBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Ray));
        hitBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Hit));

        // Geometry
        BDT_INSTANCE.BindStaticVertexBuffer();
        BDT_INSTANCE.BindSkinnedOutputBuffer();
        BDT_INSTANCE.BindIndexBuffer();
        BDT_INSTANCE.BindDescriptorBuffer();

        // Instance
        BDT_INSTANCE.BindInstanceBuffer();

        // 4. Compute Shader 바인딩
        shader->Bind();

        // 5. Compute Shader 실행
        const uint32 groupCount = (rayCount + 63) / 64;
        shader->Dispatch(groupCount, 1, 1);

        // 6. 후속 Compute Pass에서 Hit Buffer 접근을 위한 메모리 배리어
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // 7. 바인딩 해제
        ComputeShader::Unbind();
    }

    void RTClosestHitPass::Shutdown()
    {
        m_closestHitShader = {};
    }
}
