#include "pch.h"
#include "Model.h"
#include "StaticMesh.h"
#include "SkinnedMesh.h"
#include "Skeleton.h"

namespace bdt
{
	Model::Model() = default;
	Model::~Model() = default;

    ModelPtr Model::Create
    (
        ModelImportResult&& result,
        List<ResourceHandle<Material>> materials
    )
    {
        ModelPtr model(new Model());
        if (!model->Init(std::move(result), std::move(materials))) return nullptr;
        return model;
    }

    bool Model::Init
    (
        ModelImportResult&& result,
        List<ResourceHandle<Material>> materials
    )
    {
        if (result.model.nodes.empty()) return false;
        if (result.model.materials.size() != materials.size()) return false;

        // Initialize Skeleton
        SkeletonUPtr skeleton = Skeleton::Create(result.model.nodes);
        if (!skeleton) return false;

        // Register Bones with Imported IDs
        for (const ModelBone& bone : result.bones)
        {
            if (skeleton->GetNodeIndex(bone.name) != bone.nodeIndex)
                return false;

            int32 boneID = skeleton->AddBone(bone.name, bone.offset);
            if (boneID < 0 || static_cast<uint32>(boneID) != bone.id)
                return false;
        }

        // Assign Model Data
        m_data = std::move(result.model);
        m_skeleton = std::move(skeleton);
        m_materials = std::move(materials);

        return true;
    }

    bool Model::BuildMeshes()
    {
        if (m_data.meshes.empty()) return false;
        if (!m_meshes.empty()) return false;

        List<ModelMeshResource> meshes(m_data.meshes.size());

        for (usize i = 0; i < m_data.meshes.size(); ++i)
        {
            const ModelMeshData& data = m_data.meshes[i];
            const auto* staticVertices = std::get_if<List<GLStaticVertex>>(&data.vertices);
            const auto* skinnedVertices = std::get_if<List<GLSkinnedVertex>>(&data.vertices);

            // Static Mesh
            if (staticVertices)
            {
                StaticMeshPtr mesh = StaticMesh::Create(*staticVertices, data.indices);
                if (!mesh) return false;
                meshes[i] = std::move(mesh);
            }

            // Skinned Mesh
            else if (skinnedVertices)
            {
                SkinnedMeshPtr mesh = SkinnedMesh::Create(*skinnedVertices, data.indices);
                if (!mesh) return false;
                meshes[i] = std::move(mesh);
            }

            // None
            else
            {
                return false;
            }
        }

        m_meshes = std::move(meshes);
        return true;
    }

    const ModelData& Model::GetData() const
    {
        return m_data;
    }

    const Skeleton* Model::GetSkeleton() const
    {
        return m_skeleton.get();
    }

    const List<Model::ModelMeshResource>& Model::GetMeshes() const
    {
        return m_meshes;
    }

    const List<ResourceHandle<Material>>& Model::GetMaterials() const
    {
        return m_materials;
    }
}