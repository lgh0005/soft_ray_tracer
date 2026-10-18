#include "pch.h"
#include "SkinnedModelRenderer.h"
#include <Resources/Model.h>
#include <Resources/Skeleton.h>

namespace bdt
{
    SkinnedModelRenderer::SkinnedModelRenderer
    (
        Actor* owner,
        const ResourceHandle<Model>& model
    ) : Super(owner, model) { }

    SkinnedModelRenderer::~SkinnedModelRenderer() = default;

    bool SkinnedModelRenderer::ApplyPose
    (
        const List<mat4>& nodeGlobalTransforms
    )
    {
        // 모델 로딩 및 RenderObject 초기화 확인
        if (GetRenderObjects().empty())
            return false;

        // 기존 Bone Palette 갱신
        return UpdateBoneMatrices(nodeGlobalTransforms);
    }

    bool SkinnedModelRenderer::BuildRenderObjects()
    {
        ModelPtr model = GetModel();
        if (!model) return false;

        const ModelData& data = model->GetData();
        const auto& meshes = model->GetMeshes();
        const auto& materials = model->GetMaterials();

        m_renderObjects.clear();
        m_renderNodeIndices.clear();

        // Bone Palette 초기화
        if (!BuildBoneMatrices(*model))
            return false;

        usize objectCount = 0;
        for (const ModelNode& node : data.nodes)
            objectCount += node.meshIndices.size();
        m_renderObjects.reserve(objectCount);
        m_renderNodeIndices.reserve(objectCount);

        for (uint32 nodeIndex = 0; nodeIndex < data.nodes.size(); ++nodeIndex)
        {
            const ModelNode& node = data.nodes[nodeIndex];
            for (uint32 meshIndex : node.meshIndices)
            {
                if (meshIndex >= meshes.size() || meshIndex >= data.meshes.size())
                    continue;

                MeshPtr mesh;
                bool isSkinned = false;
                const auto& meshResource = meshes[meshIndex];
                const auto* staticMesh = std::get_if<StaticMeshPtr>(&meshResource);
                const auto* skinnedMesh = std::get_if<SkinnedMeshPtr>(&meshResource);
                if (staticMesh) mesh = *staticMesh;
                else if (skinnedMesh)
                {
                    mesh = *skinnedMesh;
                    isSkinned = true;
                }
                if (!mesh) continue;

                const uint32 materialIndex = data.meshes[meshIndex].materialIndex;
                if (materialIndex >= materials.size()) continue;
                const ResourceHandle<Material>& material = materials[materialIndex];

                const List<mat4>* boneMatrices = nullptr;
                if (isSkinned)
                {
                    if (meshIndex >= m_bonePalettes.size()) continue;
                    const List<mat4>& palette = m_bonePalettes[meshIndex];
                    if (palette.empty()) continue;
                    boneMatrices = &palette;
                }

                m_renderObjects.emplace_back
                (
                    mesh,
                    material,
                    mat4(1.0f),
                    isSkinned,
                    boneMatrices
                );

                m_renderNodeIndices.push_back(nodeIndex);
            }
        }

        m_renderObjectsDirty = true;
        return !m_renderObjects.empty();
    }

    bool SkinnedModelRenderer::BuildBoneMatrices(const Model& model)
    {
        const Skeleton* skeleton = model.GetSkeleton();
        if (!skeleton) return false;
        const ModelData& data = model.GetData();
        const List<ModelBone>& bones = skeleton->GetBones();

        // Mesh별 Bone Palette 초기화
        m_bonePalettes.clear();
        m_bonePalettes.resize(data.meshes.size());

        for (usize meshIndex = 0; meshIndex < data.meshes.size(); ++meshIndex)
        {
            const ModelMeshData& meshData = data.meshes[meshIndex];
            if (!std::holds_alternative<List<GLSkinnedVertex>>(meshData.vertices))
                continue;

            const auto& bindings = meshData.boneBindings;
            if (bindings.empty()) return false;

            List<mat4>& palette = m_bonePalettes[meshIndex];
            palette.resize(bindings.size(), mat4(1.0f));
            for (usize localBoneID = 0; localBoneID < bindings.size(); ++localBoneID)
            {
                const ModelMeshBoneBinding& binding = bindings[localBoneID];

                if (binding.skeletonBoneID < 0)
                    return false;

                const usize skeletonBoneID = static_cast<usize>(binding.skeletonBoneID);
                if (skeletonBoneID >= bones.size()) return false;

                const ModelBone& bone = bones[skeletonBoneID];
                if (bone.nodeIndex < 0) return false;

                const usize nodeIndex = static_cast<usize>(bone.nodeIndex);
                if (nodeIndex >= data.nodes.size()) return false;

                // 기본 포즈의 뼈 전역 변환
                const mat4& boneGlobalTransform = data.nodes[nodeIndex].globalTransform;

                // Mesh 로컬 Bone ID에 대응하는 Skinning Matrix
                palette[localBoneID] = boneGlobalTransform * binding.offset;
            }
        }

        return true;
    }

    bool SkinnedModelRenderer::UpdateBoneMatrices(const List<mat4>& nodeGlobalTransforms)
    {
        ModelPtr model = m_model.Get();
        if (!model) return false;

        const Skeleton* skeleton = model->GetSkeleton();
        if (!skeleton) return false;

        const ModelData& data = model->GetData();
        const auto& bones = skeleton->GetBones();

        if (nodeGlobalTransforms.size() != data.nodes.size()) return false;
        if (m_bonePalettes.size() != data.meshes.size()) return false;

        for (usize meshIndex = 0; meshIndex < data.meshes.size(); ++meshIndex)
        {
            const ModelMeshData& meshData = data.meshes[meshIndex];
            if (!std::holds_alternative<List<GLSkinnedVertex>>(meshData.vertices))
                continue;

            const auto& bindings = meshData.boneBindings;
            List<mat4>& palette = m_bonePalettes[meshIndex];
            if (palette.size() != bindings.size()) return false;

            for (usize localBoneID = 0; localBoneID < bindings.size(); ++localBoneID)
            {
                const ModelMeshBoneBinding& binding = bindings[localBoneID];
                if (binding.skeletonBoneID < 0 || static_cast<usize>(binding.skeletonBoneID) >= bones.size())
                    return false;

                const ModelBone& bone = bones[binding.skeletonBoneID];
                if (bone.nodeIndex < 0 || static_cast<usize>(bone.nodeIndex) >= nodeGlobalTransforms.size())
                    return false;

                palette[localBoneID] = nodeGlobalTransforms[bone.nodeIndex] * binding.offset;
            }
        }

        return true;
    }

    void SkinnedModelRenderer::UpdateWorldTransforms()
    {
        if (!m_renderObjectsDirty) return;

        ModelPtr model = GetModel();
        if (!model) return;

        const auto& nodes = model->GetData().nodes;
        const mat4 worldMatrix = GetTransformMatrix();
        for (usize i = 0; i < m_renderObjects.size(); ++i)
        {
            const uint32 nodeIndex = m_renderNodeIndices[i];

            if (m_renderObjects[i].IsSkinned())
            {
                // 스키닝 행렬에서 Node Transform을 반영
                m_renderObjects[i].SetWorldMatrix(worldMatrix);
            }
            else
            {
                // 혼합 모델의 정적 메쉬 처리
                m_renderObjects[i].SetWorldMatrix(worldMatrix * nodes[nodeIndex].globalTransform);
            }
        }

        m_renderObjectsDirty = false;
    }
}
