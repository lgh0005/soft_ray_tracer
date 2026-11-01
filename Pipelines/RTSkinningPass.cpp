#include "pch.h"
#include "RTSkinningPass.h"

namespace bdt
{
    RTSkinningPass::RTSkinningPass() = default;

    RTSkinningPass::~RTSkinningPass()
    {
        Shutdown();
    }

    RTSkinningPassUPtr RTSkinningPass::Create()
    {
        RTSkinningPassUPtr result(new RTSkinningPass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTSkinningPass::Init()
    {
        m_skinningShader = BDT_RESOURCE.LoadComputeShader(
            "RT.Skinning",
            { { GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_skinning.comp" } }
        );

        return true;
    }

    void RTSkinningPass::Execute(IRenderBus& bus)
    {
        // 1. Shader 확인
        const auto shader = m_skinningShader.Get();
        if (!shader || !shader->IsValid()) return;

        // 2. Instance / Vertex Count
        const uint32 instanceCount = BDT_INSTANCE.GetInstanceCount();
        const uint32 maxVertexCount = BDT_INSTANCE.GetMaxSkinnedVertexCount();
        if (instanceCount == 0 || maxVertexCount == 0) return;

        // 3. Geometry Input
        BDT_INSTANCE.BindSkinnedVertexBuffer();
        BDT_INSTANCE.BindDescriptorBuffer();

        // 4. Instance / Bone Input
        BDT_INSTANCE.BindInstanceBuffer();
        BDT_INSTANCE.BindBoneBuffer();

        // 5. Skinning Output
        BDT_INSTANCE.BindSkinnedOutputBuffer();

        // 6. Compute Shader
        shader->Bind();

        // X: Vertex Index
        // Y: Instance Index
        const uint32 groupX = (maxVertexCount + 63u) / 64u;
        const uint32 groupY = instanceCount;

        shader->Dispatch(groupX, groupY, 1);

        // 7. Skinning Output -> ClosestHit / Shading
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        ComputeShader::Unbind();
    }

    void RTSkinningPass::Shutdown()
    {
        m_skinningShader = {};
    }
}
