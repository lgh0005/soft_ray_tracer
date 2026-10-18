#pragma once
#include "Importer.h"
#include "ModelData.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

namespace bdt
{
    class ModelImporter final : public Importer
    {
        using Super = Importer;

    public:
        ModelImporter();
        virtual ~ModelImporter() override;
        bool Init(const String& path);

        uint32 GetMeshCount() const;
        uint32 GetMaterialCount() const;

        bool ProcessNodes(List<ModelNode>& output) const;
        bool ProcessBones(List<ModelBoneSource>& output) const;
        bool ProcessMesh(uint32 meshIndex, ModelMeshData& output) const;
        bool ProcessMaterial(uint32 materialIndex, MaterialDesc& desc, MaterialTexturePaths& paths) const;

        static bool GetImportResult(const String& path, ModelImportParts&& parts, ModelImportResult& output);

    private:
        static int32 AppendNode(const aiNode* node, int32 parentIndex, List<ModelNode>& nodes);
        static bool ExtractMesh(const aiMesh* mesh, ModelMeshData& output);
        static bool ExtractMaterial(const aiMaterial* material, const String& path, MaterialDesc& outputDesc, MaterialTexturePaths& outputPaths);
        static void AddBoneWeight(GLSkinnedVertex& vertex, int32 boneID, float weight);
        static void NormalizeBoneWeights(GLSkinnedVertex& vertex);
        static String GetTexturePath(const aiMaterial* material, int32 textureType, const String& modelPath);
    };
}

