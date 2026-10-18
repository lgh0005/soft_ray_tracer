#include "pch.h"
#include "ModelRenderer.h"
#include <Resources/Model.h>

namespace bdt
{
    ModelRenderer::ModelRenderer
    (
        Actor* owner,
        const ResourceHandle<Model>& model
    ) : Super(owner), m_model(model) { }

    ModelRenderer::~ModelRenderer() = default;

    const List<RenderObject>& ModelRenderer::GetRenderObjects()
    {
        // 비동기 로딩 미완료
        ModelPtr model = m_model.Get();
        if (!model) return m_renderObjects;

        // RenderObject 최초 생성
        if (m_renderObjects.empty())
        {
            if (!BuildRenderObjects())
                return m_renderObjects;
        }

        // 최종 World Transform 갱신
        UpdateWorldTransforms();
        return m_renderObjects;
    }

    ModelPtr ModelRenderer::GetModel() const
    {
        return m_model.Get();
    }

    bool ModelRenderer::BuildRenderObjects()
    {
        ModelPtr model = m_model.Get();
        if (!model) return false;

        const ModelData& data = model->GetData();
        const auto& meshes = model->GetMeshes();
        const auto& materials = model->GetMaterials();

        // 기존 캐시 초기화
        m_renderObjects.clear();
        m_renderNodeIndices.clear();

        // 실제 RenderObject 개수 계산
        usize objectCount = 0;
        for (const ModelNode& node : data.nodes) objectCount += node.meshIndices.size();
        m_renderObjects.reserve(objectCount);
        m_renderNodeIndices.reserve(objectCount);

        // RenderOject 빌드
        for (uint32 nodeIndex = 0; nodeIndex < data.nodes.size(); ++nodeIndex)
        {
            const ModelNode& node = data.nodes[nodeIndex];
            for (uint32 meshIndex : node.meshIndices)
            {
                // Mesh Index 검사
                if (meshIndex >= meshes.size() || meshIndex >= data.meshes.size())
                    continue;

                // 정적 메쉬만 처리
                const auto* staticMesh = std::get_if<StaticMeshPtr>(&meshes[meshIndex]);
                if (!staticMesh || !(*staticMesh)) continue;
                MeshPtr mesh = *staticMesh;

                // 머티리얼 조회
                const uint32 materialIndex = data.meshes[meshIndex].materialIndex;
                if (materialIndex >= materials.size()) continue;
                const ResourceHandle<Material>& material = materials[materialIndex];

                // RenderObject 생성
                m_renderObjects.emplace_back
                (
                    mesh,
                    material,
                    mat4(1.0f),
                    false,
                    nullptr
                );

                m_renderNodeIndices.push_back(nodeIndex);
            }
        }

        m_renderObjectsDirty = true;
        return !m_renderObjects.empty();
    }

    void ModelRenderer::UpdateWorldTransforms()
    {
        if (!m_renderObjectsDirty) return;

        ModelPtr model = m_model.Get();
        if (!model) return;

        const auto& nodes = model->GetData().nodes;
        const mat4 worldMatrix = GetTransformMatrix();
        for (usize i = 0; i < m_renderObjects.size(); ++i)
        {
            const uint32 nodeIndex = m_renderNodeIndices[i];
            m_renderObjects[i].SetWorldMatrix(worldMatrix * nodes[nodeIndex].globalTransform);
        }

        m_renderObjectsDirty = false;
    }

    void ModelRenderer::MarkTransformDirty()
    {
        Super::MarkTransformDirty();
        m_renderObjectsDirty = true;
    }
}
