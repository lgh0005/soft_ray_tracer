#include "pch.h"
#include "ModelImporter.h"
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>

namespace bdt
{
	ModelImporter::ModelImporter() = default;
	ModelImporter::~ModelImporter() = default;

    bool ModelImporter::Init(const String& path)
    {
        constexpr uint32 flags =
            aiProcess_Triangulate           |
            aiProcess_GenSmoothNormals      |
            aiProcess_CalcTangentSpace      |
            aiProcess_JoinIdenticalVertices |
            aiProcess_LimitBoneWeights      |
            aiProcess_FlipUVs;

        return Super::LoadScene(path, flags);
    }

    uint32 ModelImporter::GetMeshCount() const
    {
        const aiScene* scene = GetScene();
        return scene ? scene->mNumMeshes : 0;
    }

    uint32 ModelImporter::GetMaterialCount() const
    {
        const aiScene* scene = GetScene();
        return scene ? scene->mNumMaterials : 0;
    }

    /*======================================//
    //   independent data extraction tasks  //
    //======================================*/
    bool ModelImporter::ProcessNodes(List<ModelNode>& output) const
    {
        const aiScene* scene = GetScene();
        if (!scene || !scene->mRootNode) return false;

        List<ModelNode> nodes;
        if (AppendNode(scene->mRootNode, -1, nodes) < 0) return false;
        output = std::move(nodes);

        return true;
    }

    bool ModelImporter::ProcessBones(List<ModelBoneSource>& output) const
    {
        const aiScene* scene = GetScene();

        if (!scene) return false;

        List<ModelBoneSource> bones;
        Dictionary<String, uint32> registered;

        for (uint32 i = 0; i < scene->mNumMeshes; ++i)
        {
            const aiMesh* mesh = scene->mMeshes[i];
            if (!mesh) return false;

            for (uint32 j = 0; j < mesh->mNumBones; ++j)
            {
                const aiBone* bone = mesh->mBones[j];
                if (!bone) return false;

                String name = bone->mName.C_Str();

                if (registered.find(name) != registered.end())
                    continue;

                ModelBoneSource data;
                data.name = name;
                data.offset = ToMatrix(bone->mOffsetMatrix);

                registered.emplace(name, static_cast<uint32>(bones.size()));
                bones.push_back(std::move(data));
            }
        }

        output = std::move(bones);
        return true;
    }

    bool ModelImporter::ProcessMesh(uint32 meshIndex, ModelMeshData& output) const
    {
        const aiScene* scene = GetScene();
        if (!scene || meshIndex >= scene->mNumMeshes) return false;
        return ExtractMesh(scene->mMeshes[meshIndex], output);
    }

    bool ModelImporter::ProcessMaterial(uint32 materialIndex, MaterialDesc& desc, MaterialTexturePaths& paths) const
    {
        const aiScene* scene = GetScene();
        if (!scene || materialIndex >= scene->mNumMaterials) return false;
        return ExtractMaterial(scene->mMaterials[materialIndex], GetPath(), desc, paths);
    }

    /*======================================//
    //   imported data assembly and linking //
    //======================================*/
    bool ModelImporter::GetImportResult(const String& path, ModelImportParts&& parts, ModelImportResult& output)
    {
        if (parts.nodes.empty()) return false;

        // Node Name -> Node Index
        Dictionary<String, int32> nodeIndices;
        for (usize i = 0; i < parts.nodes.size(); ++i)
        {
            const String& name = parts.nodes[i].name;
            if (!nodeIndices.emplace(name, static_cast<int32>(i)).second)
                return false;
        }

        // Bone Name -> Offset Matrix
        Dictionary<String, mat4> boneOffsets;
        for (const ModelBoneSource& source : parts.bones)
        {
            if (!boneOffsets.emplace(source.name, source.offset).second)
                return false;
        }

        // Skeleton Bone ID
        Dictionary<String, int32> boneIDs;
        List<ModelBone> bones;

        for (usize i = 0; i < parts.nodes.size(); ++i)
        {
            const ModelNode& node = parts.nodes[i];
            auto it = boneOffsets.find(node.name);
            if (it == boneOffsets.end()) continue;

            ModelBone bone;
            bone.name = node.name;
            bone.id = static_cast<uint32>(bones.size());
            bone.nodeIndex = static_cast<int32>(i);
            bone.offset = it->second;

            boneIDs.emplace(bone.name, static_cast<int32>(bone.id));
            bones.push_back(std::move(bone));
        }

        if (bones.size() != boneOffsets.size())
            return false;

        // Mesh Bone Palette -> Skeleton Bone ID
        for (ModelMeshData& mesh : parts.meshes)
        {
            if (!parts.materials.empty() && mesh.materialIndex >= parts.materials.size())
                return false;

            if (!mesh.boneBindings.empty() &&
                !std::holds_alternative<List<GLSkinnedVertex>>(mesh.vertices))
                return false;

            for (ModelMeshBoneBinding& binding : mesh.boneBindings)
            {
                auto it = boneIDs.find(binding.boneName);
                if (it == boneIDs.end()) return false;
                binding.skeletonBoneID = it->second;
            }
        }

        // Assemble Result
        ModelImportResult result;
        result.model.path = path;
        result.model.nodes = std::move(parts.nodes);
        result.model.meshes = std::move(parts.meshes);
        result.model.materials = std::move(parts.materials);
        result.bones = std::move(bones);

        output = std::move(result);
        return true;
    }

    /*=====================================//
    //   internal data extraction helpers  //
    //=====================================*/
    int32 ModelImporter::AppendNode(const aiNode* node, int32 parentIndex, List<ModelNode>& nodes)
    {
        if (!node) return -1;

        int32 nodeIndex = static_cast<int32>(nodes.size());

        ModelNode data;
        data.name = node->mName.C_Str();
        data.parentIndex = parentIndex;
        data.localTransform = ToMatrix(node->mTransformation);

        // 기본 Local Pose 추출
        aiVector3D position;
        aiQuaternion rotation;
        aiVector3D scale;
        node->mTransformation.Decompose(scale, rotation, position);
        data.localPose = Pose
        (
            vec3(position.x, position.y, position.z),
            quat(rotation.w, rotation.x, rotation.y, rotation.z),
            vec3(scale.x, scale.y, scale.z)
        );

        // 부모 계층 변환 누적
        if (parentIndex >= 0) data.globalTransform = nodes[parentIndex].globalTransform * data.localTransform;
        else data.globalTransform = data.localTransform;

        data.meshIndices.reserve(node->mNumMeshes);
        for (uint32 i = 0; i < node->mNumMeshes; ++i)
            data.meshIndices.push_back(node->mMeshes[i]);

        nodes.push_back(std::move(data));

        if (parentIndex >= 0)
            nodes[parentIndex].children.push_back(static_cast<uint32>(nodeIndex));

        for (uint32 i = 0; i < node->mNumChildren; ++i)
            AppendNode(node->mChildren[i], nodeIndex, nodes);

        return nodeIndex;
    }

    bool ModelImporter::ExtractMesh(const aiMesh* mesh, ModelMeshData& output)
    {
        if (!mesh || !mesh->HasPositions()) return false;

        ModelMeshData data;

        data.name = mesh->mName.C_Str();
        data.materialIndex = mesh->mMaterialIndex;

        // Vertex Type
        if (mesh->HasBones()) data.vertices.emplace<List<GLSkinnedVertex>>(mesh->mNumVertices);
        else data.vertices.emplace<List<GLStaticVertex>>(mesh->mNumVertices);

        // Vertex Data
        std::visit
        (
            [&](auto& vertices)
            {
                for (uint32 i = 0; i < mesh->mNumVertices; ++i)
                {
                    auto& vertex = vertices[i];
                    vertex.position = ToVector(mesh->mVertices[i]);

                    if (mesh->HasNormals())
                        vertex.normal = ToVector(mesh->mNormals[i]);

                    if (mesh->HasTextureCoords(0))
                    {
                        const aiVector3D& uv = mesh->mTextureCoords[0][i];
                        vertex.texCoord = vec2(uv.x, uv.y);
                    }

                    if (mesh->HasTangentsAndBitangents())
                        vertex.tangent = ToVector(mesh->mTangents[i]);
                }
            },
            data.vertices
        );

        // Bone Palette
        if (mesh->HasBones())
        {
            auto& vertices = std::get<List<GLSkinnedVertex>>(data.vertices);

            data.boneBindings.reserve(mesh->mNumBones);
            for (uint32 i = 0; i < mesh->mNumBones; ++i)
            {
                const aiBone* bone = mesh->mBones[i];
                if (!bone) return false;

                ModelMeshBoneBinding binding;
                binding.boneName = bone->mName.C_Str();
                binding.offset = ToMatrix(bone->mOffsetMatrix);

                data.boneBindings.push_back(std::move(binding));
                int32 localBoneID = static_cast<int32>(i);
                for (uint32 j = 0; j < bone->mNumWeights; ++j)
                {
                    const aiVertexWeight& influence = bone->mWeights[j];
                    if (influence.mVertexId >= vertices.size())
                        return false;

                    AddBoneWeight
                    (
                        vertices[influence.mVertexId],
                        localBoneID,
                        influence.mWeight
                    );
                }
            }

            for (GLSkinnedVertex& vertex : vertices)
                NormalizeBoneWeights(vertex);
        }

        // Index Data
        data.indices.reserve(mesh->mNumFaces * 3);
        for (uint32 i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];
            if (face.mNumIndices != 3) return false;
            for (uint32 j = 0; j < face.mNumIndices; ++j)
            {
                if (face.mIndices[j] >= mesh->mNumVertices) return false;
                data.indices.push_back(face.mIndices[j]);
            }
        }

        output = std::move(data);
        return true;
    }

    bool ModelImporter::ExtractMaterial(const aiMaterial* material, const String& path, MaterialDesc& outputDesc, MaterialTexturePaths& outputPaths)
    {
        if (!material) return false;

        MaterialDesc desc;
        MaterialTexturePaths paths;

        // 기본 슬롯의 실제 경로가 없을 때만 호환 슬롯을 순서대로 조회한다.
        paths.albedo = GetTexturePath(material, { aiTextureType_BASE_COLOR, aiTextureType_DIFFUSE }, path);
        paths.normal = GetTexturePath(material, { aiTextureType_NORMAL_CAMERA, aiTextureType_NORMALS }, path);
        paths.ao = GetTexturePath(material, { aiTextureType_AMBIENT_OCCLUSION, aiTextureType_LIGHTMAP }, path);
        int32 roughnessType = -1;
        paths.roughness = GetTexturePath
        (
            material,
            { aiTextureType_DIFFUSE_ROUGHNESS, aiTextureType_MAYA_SPECULAR_ROUGHNESS, aiTextureType_SHININESS },
            path,
            &roughnessType
        );
        const bool usesLegacyRoughness = roughnessType == aiTextureType_SHININESS;
        paths.metallic = GetTexturePath(material, { aiTextureType_METALNESS }, path);

        // 기존 FBX 변환 경로에서만 원본 Specular를 사용하며 파일명을 추측하지 않는다.
        if (usesLegacyRoughness && !paths.metallic.empty())
            paths.specular = GetTexturePath(material, { aiTextureType_SPECULAR, aiTextureType_MAYA_SPECULAR_COLOR }, path);
        paths.emission = GetTexturePath(material, { aiTextureType_EMISSION_COLOR, aiTextureType_EMISSIVE }, path);
        paths.height = GetTexturePath(material, { aiTextureType_HEIGHT, aiTextureType_DISPLACEMENT }, path);

        // Base Color
        aiColor4D color;

        if (material->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS ||
            material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
            desc.albedoFactor = vec4(color.r, color.g, color.b, color.a);

        // Emissive Color
        if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
            desc.emissiveFactor = vec3(color.r, color.g, color.b);

        // PBR Factors
        float value{ 0.0f };
        if (material->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS)
            desc.metallicFactor = value;
        else if (!paths.metallic.empty())
            desc.metallicFactor = 1.0f;

        // SHININESS 호환 슬롯은 프로젝트의 독립 Roughness 입력으로 사용하며 배율은 1을 유지한다.
        if (!usesLegacyRoughness && material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS)
            desc.roughnessFactor = value;
        if (material->Get(AI_MATKEY_EMISSIVE_INTENSITY, value) == AI_SUCCESS) desc.emissionStrength = value;

        outputDesc = std::move(desc);
        outputPaths = std::move(paths);
        return true;
    }

    void ModelImporter::AddBoneWeight(GLSkinnedVertex& vertex, int32 boneID, float weight)
    {
        if (boneID < 0 || weight <= 0.0f) return;

        for (int32 i = 0; i < 4; ++i)
        {
            if (weight <= vertex.weights[i]) continue;
            for (int32 j = 3; j > i; --j)
            {
                vertex.boneIDs[j] = vertex.boneIDs[j - 1];
                vertex.weights[j] = vertex.weights[j - 1];
            }

            vertex.boneIDs[i] = boneID;
            vertex.weights[i] = weight;
            return;
        }
    }

    void ModelImporter::NormalizeBoneWeights(GLSkinnedVertex& vertex)
    {
        float sum{ 0.0f };

        for (float weight : vertex.weights) sum += weight;
        if (sum <= 0.0f) return;

        for (float& weight : vertex.weights)
            weight /= sum;
    }

    String ModelImporter::GetTexturePath
    (
        const aiMaterial* material,
        std::initializer_list<int32> textureTypes,
        const String& modelPath,
        int32* selectedType
    )
    {
        if (selectedType) *selectedType = -1;
        if (!material) return {};

        const std::filesystem::path directory = std::filesystem::path(modelPath).parent_path();
        for (int32 type : textureTypes)
        {
            const auto textureType = static_cast<aiTextureType>(type);
            for (uint32 index = 0; index < material->GetTextureCount(textureType); ++index)
            {
                aiString texturePath;
                if (material->GetTexture(textureType, index, &texturePath) != AI_SUCCESS) continue;
                String path = texturePath.C_Str();
                if (path.empty()) continue;

                // Embedded 참조는 기존 계약대로 보존한다. 실제 로딩 지원은 별도 경로다.
                if (path[0] == '*')
                {
                    if (selectedType) *selectedType = type;
                    return path;
                }

                // Image2D는 KTX2를 사용하므로 원본 경로 구조를 유지해 확장자만 변환한다.
                std::filesystem::path texture(path);
                if (texture.filename().empty()) continue;
                texture.replace_extension(".ktx2");
                const std::filesystem::path candidates[] =
                {
                    texture.is_absolute() ? texture : directory / texture,
                    directory / texture.filename()
                };
                for (const auto& candidate : candidates)
                {
                    const auto normalized = candidate.lexically_normal();
                    std::error_code error;
                    if (!std::filesystem::is_regular_file(normalized, error)) continue;
                    if (selectedType) *selectedType = type;
                    return normalized.string();
                }
            }
        }
        return {};
    }
}
