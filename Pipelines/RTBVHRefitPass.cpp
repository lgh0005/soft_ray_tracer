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

        m_levelRefitShader = BDT_RESOURCE.LoadComputeShader
        (
            "RT.BVH.Refit.Level",
            {{GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_bvh_refit_level.comp"}}
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

        //[수정] Storage prepares dispatch data; the pass owns shaders and submission.
        RTBVHRefitData data;
        if (!builder->PrepareRefit(data, m_parallelInternalRefit)) return false;
        if (data.instanceCount == 0)
        {
            builder->CompleteRefit();
            return true;
        }
        if (!data.refitTLAS) return true;
        if (!AreShadersReady(data.refitBLAS)) return false;

        BDT_INSTANCE.BindIndexBuffer();
        BDT_INSTANCE.BindDescriptorBuffer();
        BDT_INSTANCE.BindInstanceBuffer();
        BDT_INSTANCE.BindSkinnedOutputBuffer();
        bvh->Bind();

        if (data.refitBLAS && data.maxSkinnedNodeCount != 0) RefitBLAS(*bvh, data);
        RefitTLAS(data);
        ComputeShader::Unbind();
        builder->CompleteRefit();
        return true;
    }

    bool RTBVHRefitPass::AreShadersReady(bool refitBLAS) const
    {
        const auto tlasLeaf = m_tlasLeafRefitShader.Get();
        const auto tlasInternal = m_tlasInternalRefitShader.Get();
        if (!tlasLeaf || !tlasLeaf->IsValid() || !tlasInternal || !tlasInternal->IsValid()) return false;
        if (!refitBLAS) return true;
        const auto leaf = m_leafRefitShader.Get();
        if (!leaf || !leaf->IsValid()) return false;
        if (m_parallelInternalRefit)
        {
            const auto level = m_levelRefitShader.Get();
            return level && level->IsValid();
        }
        const auto internal = m_internalRefitShader.Get();
        return internal && internal->IsValid();
    }

    void RTBVHRefitPass::RefitBLAS(RTBVHStorage& storage, const RTBVHRefitData& data)
    {
        const auto leaf = m_leafRefitShader.Get();
        leaf->Bind();
        leaf->Dispatch((data.maxSkinnedNodeCount + 63u) / 64u, data.instanceCount, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        //[수정] Deepest level first, or the serial reference.
        if (m_parallelInternalRefit)
        {
            const auto level = m_levelRefitShader.Get();
            level->Bind();
            storage.BindRefitLevelTasks();
            for (const uvec2& range : storage.GetRefitLevelRanges())
            {
                glUniform1ui(0, range.x);
                glUniform1ui(1, range.y);
                level->Dispatch((range.y + 63u) / 64u, data.instanceCount, 1);
                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            }
        }
        else
        {
            const auto internal = m_internalRefitShader.Get();
            internal->Bind();
            internal->Dispatch((data.instanceCount + 63u) / 64u, 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
    }

    void RTBVHRefitPass::RefitTLAS(const RTBVHRefitData& data)
    {
        const auto leaf = m_tlasLeafRefitShader.Get();
        leaf->Bind();
        leaf->Dispatch((data.tlasNodeCount + 63u) / 64u, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        const auto internal = m_tlasInternalRefitShader.Get();
        internal->Bind();
        internal->Dispatch(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void RTBVHRefitPass::Shutdown()
    {
        m_leafRefitShader = {};
        m_internalRefitShader = {};
        m_levelRefitShader = {};
        m_tlasLeafRefitShader = {};
        m_tlasInternalRefitShader = {};
    }

    void RTBVHRefitPass::SetParallelInternalRefit(bool enabled)
    {
        m_parallelInternalRefit = enabled;
    }

}
