#pragma once
#include <Core/Types.h>
#include <Core/GLVertices.h>
#include <variant>
#include "MaterialData.h"
#include "Pose.h"

namespace bdt
{
    using ModelVertexData = std::variant
    <
        List<GLStaticVertex>, 
        List<GLSkinnedVertex>
    >;

    struct ModelNode
    {
        String name;
        int32 parentIndex{ -1 };
        Pose localPose{vec3(0.0f), quat(1.0f, 0.0f, 0.0f, 0.0f), vec3(1.0f)};
        mat4 localTransform{ 1.0f };
        mat4 globalTransform{ 1.0f };
        List<uint32> meshIndices;
        List<uint32> children;
    };

    struct ModelBone
    {
        String name;
        uint32 id{ 0 };
        int32 nodeIndex{ -1 };
        mat4 offset{ 1.0f };
    };

    struct ModelBoneSource
    {
        String name;
        mat4 offset{ 1.0f };
    };

    struct ModelMeshBoneBinding
    {
        String boneName;
        int32 skeletonBoneID{ -1 };
        mat4 offset{ 1.0f };
    };

    struct ModelMeshData
    {
        String name;
        uint32 materialIndex{ 0 };
        ModelVertexData vertices;
        List<uint32> indices;
        List<ModelMeshBoneBinding> boneBindings;
    };

    struct ModelImportParts
    {
        List<ModelNode> nodes;
        List<ModelBoneSource> bones;
        List<ModelMeshData> meshes;
        List<MaterialDesc> materials;
        List<MaterialTexturePaths> materialTexturePaths;
    };

    struct ModelData
    {
        String path;
        List<ModelNode> nodes;
        List<ModelMeshData> meshes;
        List<MaterialDesc> materials;
    };

    struct ModelImportResult
    {
        ModelData model;
        List<ModelBone> bones;
    };
}