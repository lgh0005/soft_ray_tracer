#include "pch.h"
#include "RTBVHRefitPass.h"
#include "RTBVHBus.h"
#include <RayTracing/RTBVHStorage.h>
#include <RayTracing/InstanceManager.h>

namespace bdt
{
    RTBVHRefitPass::RTBVHRefitPass() = default;

    RTBVHRefitPass::~RTBVHRefitPass()
    {
        Shutdown();
    }

    RTBVHRefitPassUPtr RTBVHRefitPass::Create()
    {
        RTBVHRefitPassUPtr result(new RTBVHRefitPass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTBVHRefitPass::Init()
    {
        m_leafRefitShader = BDT_RESOURCE.LoadComputeShader
        (
            "RT.BVH.Refit.Leaf",
            {{GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_bvh_refit_leaf.comp"}}
        );

        m_internalRefitShader = BDT_RESOURCE.LoadComputeShader
        (
            "RT.BVH.Refit.Internal",
            {{GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_bvh_refit_internal.comp"}}
        );

        m_tlasLeafRefitShader = BDT_RESOURCE.LoadComputeShader
        (
            "RT.BVH.Refit.TLAS.Leaf",
            {{GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_bvh_refit_tlas_leaf.comp"}}
        );

        m_tlasInternalRefitShader = BDT_RESOURCE.LoadComputeShader
        (
            "RT.BVH.Refit.TLAS.Internal",
            {{GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_bvh_refit_tlas_internal.comp"}}
        );

        return true;
    }

    bool RTBVHRefitPass::Execute(IRenderBus& bus)
    {
        auto& rtBus = static_cast<RTBVHBus&>(bus);
        RTBVHStorage* bvh = rtBus.GetBVHStorage();
        RTBVHBuilder* builder = rtBus.GetBVHBuilder();
        if (!bvh || !builder) return false;

        const List<GPUInstance>& instances = BDT_INSTANCE.GetInstances();
        const uint32 instanceCount = static_cast<uint32>(instances.size());
        if (instanceCount == 0)
        {
            builder->CompleteRefit();
            return true;
        }

        if (!builder->IsTLASRefitRequired()) return true;
        const uint32 maxNodeCount = builder->GetMaxSkinnedNodeCount();

        const TLAS* tlas = bvh->GetTLAS();
        const uint32 tlasDescriptorIndex = bvh->GetTLASDescriptorIndex();
        if (!tlas || tlas->IsEmpty() || tlasDescriptorIndex == MaxValue<uint32>) return false;

        const auto tlasLeafShader = m_tlasLeafRefitShader.Get();
        const auto tlasInternalShader = m_tlasInternalRefitShader.Get();
        if (!tlasLeafShader || !tlasLeafShader->IsValid()) return false;
        if (!tlasInternalShader || !tlasInternalShader->IsValid()) return false;

        const auto leafShader = m_leafRefitShader.Get();
        const auto internalShader = m_internalRefitShader.Get();
        if (builder->IsBLASRefitRequired())
        {
            if (!leafShader || !leafShader->IsValid()) return false;
            if (!internalShader || !internalShader->IsValid()) return false;
        }

        // 2. Geometry / Instance / BVH Buffers
        BDT_INSTANCE.BindIndexBuffer();
        BDT_INSTANCE.BindDescriptorBuffer();
        BDT_INSTANCE.BindInstanceBuffer();
        BDT_INSTANCE.BindSkinnedOutputBuffer();
        bvh->Bind();

        if (builder->IsBLASRefitRequired() && maxNodeCount != 0)
        {
            // 3. Leaf Refit: X는 노드, Y는 Instance
            leafShader->Bind();
            const uint32 leafGroupX = (maxNodeCount + 63u) / 64u;
            leafShader->Dispatch(leafGroupX, instanceCount, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

            // 4. Internal Refit: Instance마다 하나의 Invocation이 역순 처리
            internalShader->Bind();
            const uint32 internalGroupX = (instanceCount + 63u) / 64u;
            internalShader->Dispatch(internalGroupX, 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

        // 5. TLAS Leaf: Instance별 BLAS Root Bounds를 World Space로 변환
        tlasLeafShader->Bind();
        const uint32 tlasLeafGroupX = (tlas->GetNodeCount() + 63u) / 64u;
        tlasLeafShader->Dispatch(tlasLeafGroupX, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // 6. TLAS Internal: 전위순 노드를 역순으로 병합
        tlasInternalShader->Bind();
        tlasInternalShader->Dispatch(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        ComputeShader::Unbind();
        builder->CompleteRefit();
        return true;
    }

    void RTBVHRefitPass::Shutdown()
    {
        m_leafRefitShader = {};
        m_internalRefitShader = {};
        m_tlasLeafRefitShader = {};
        m_tlasInternalRefitShader = {};
    }
}
