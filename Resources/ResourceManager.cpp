#include "pch.h"
#include "ResourceManager.h"
#include <Core/TaskBatch.h>
#include "ModelImporter.h"
#include "AnimImporter.h"

namespace bdt
{
    ResourceManager::ResourceManager() = default;
    ResourceManager::~ResourceManager() = default;

    ResourceHandle<Image2D> ResourceManager::LoadImage2D(const String& path)
    {
        return Load(path, m_images2D, [path]() { return Image2D::Create(path); }, true);
    }

    ResourceHandle<ImageCube> ResourceManager::LoadImageCube(const String& path)
    {
        return Load(path, m_imagesCube, [path]() { return ImageCube::Create(path); }, true);
    }

    ResourceHandle<GraphicsShader> ResourceManager::LoadGraphicsShader(const String& key, List<ShaderFile> files)
    {
        return Load(key, m_graphicsShaders, [files = std::move(files)]() { return GraphicsShader::Create(files); }, true);
    }

    ResourceHandle<ComputeShader> ResourceManager::LoadComputeShader(const String& key, List<ShaderFile> files)
    {
        return Load(key, m_computeShaders, [files = std::move(files)]() { return ComputeShader::Create(files); }, true);
    }

    ResourceHandle<Material> ResourceManager::LoadMaterial(const String& key, const MaterialDesc& desc,  const MaterialTextures& textures)
    {
        std::shared_ptr<Promise<MaterialPtr>> promise;
        ResourceHandle<Material> handle = Reserve(key, m_materials, promise);
        if (!promise) return handle;
        promise->set_value(Material::Create(desc, textures));
        return handle;
    }

    ResourceHandle<Animation> ResourceManager::LoadAnimation(const String& path)
    {
        return Load
        (
            path,
            m_animations,
            [path]() -> AnimationPtr
            {
                AnimationImporter importer;

                // FBX 파일 로드
                if (!importer.Init(path))
                    return nullptr;

                // 애니메이션 존재 여부 확인
                if (importer.GetAnimationCount() == 0)
                    return nullptr;

                // 첫 번째 애니메이션 추출
                AnimationData data;
                if (!importer.ProcessAnimation(0, data))
                    return nullptr;

                // Animation 리소스 생성
                return Animation::Create(std::move(data));
            },
            false
        );
    }

    ResourceHandle<Model> ResourceManager::LoadModel(const String& path)
    {
        std::shared_ptr<Promise<ModelPtr>> promise;
        ResourceHandle<Model> handle = Reserve(path, m_models, promise);
        if (!promise) return handle;

        auto importer = std::make_shared<ModelImporter>();
        auto parts = std::make_shared<ModelImportParts>();

        // 1. Assimp Scene 로드
        bool submitted = BDT_TASK.Submit
        (
            [importer, path]() { return importer->Init(path); },
            [this, path, importer, parts, promise](bool success)
            {
                if (success) ExtractModel(path, importer, parts, promise);
                else promise->set_value(nullptr);
            }
        );

        if (!submitted)
            promise->set_value(nullptr);

        return handle;
    }

    void ResourceManager::ExtractModel
    (
        const String& path,
        const std::shared_ptr<ModelImporter>& importer,
        const std::shared_ptr<ModelImportParts>& parts,
        const std::shared_ptr<Promise<ModelPtr>>& promise
    )
    {
        parts->meshes.resize(importer->GetMeshCount());
        parts->materials.resize(importer->GetMaterialCount());
        parts->materialTexturePaths.resize(importer->GetMaterialCount());

        // 2. 노드 / 본 / 메시 / 머티리얼 병렬 추출
        List<Func<bool>> tasks;
        tasks.emplace_back([importer, parts]() { return importer->ProcessNodes(parts->nodes); });
        tasks.emplace_back([importer, parts]() { return importer->ProcessBones(parts->bones); });

        for (uint32 i = 0; i < importer->GetMeshCount(); ++i)
            tasks.emplace_back([importer, parts, i]() { return importer->ProcessMesh(i, parts->meshes[i]); });

        for (uint32 i = 0; i < importer->GetMaterialCount(); ++i)
            tasks.emplace_back([importer, parts, i]() { return importer->ProcessMaterial(i, parts->materials[i], parts->materialTexturePaths[i]); });

        auto batch = std::make_shared<TaskBatch>
        (
            std::move(tasks),
            [this, path, parts, promise](bool success)
            {
                if (success) QueueModelBuild(path, parts, promise);
                else promise->set_value(nullptr);
            }
        );

        // 일 묶음 제출
        if (!batch->Submit())
            promise->set_value(nullptr);
    }

    void ResourceManager::QueueModelBuild
    (
        const String& path,
        const std::shared_ptr<ModelImportParts>& parts,
        const std::shared_ptr<Promise<ModelPtr>>& promise
    )
    {
        // 3. 데이터 조립 + GL Mesh 생성
        bool queued = BDT_TASK.Submit([this, path, parts, promise]() {  return BuildModel(path, parts, promise); });
        if (!queued) promise->set_value(nullptr);
    }

    bool ResourceManager::BuildModel
    (
        const String& path,
        const std::shared_ptr<ModelImportParts>& parts,
        const std::shared_ptr<Promise<ModelPtr>>& promise
    )
    {
        ModelPtr model;
        ModelImportResult result;

        // 텍스처 경로만 먼저 분리
        List<MaterialTexturePaths> texturePaths = std::move(parts->materialTexturePaths);

        if (ModelImporter::GetImportResult(path, std::move(*parts), result))
        {
            List<ResourceHandle<Material>> materials;
            materials.reserve(result.model.materials.size());

            // 모델 머티리얼 생성
            for (usize i = 0; i < result.model.materials.size(); ++i)
            {
                const MaterialDesc& desc = result.model.materials[i];
                const MaterialTexturePaths& paths = texturePaths[i];

                MaterialTextures textures;
                textures.albedo    = LoadImage2D(paths.albedo);
                textures.normal    = LoadImage2D(paths.normal);
                textures.ao        = LoadImage2D(paths.ao);
                textures.roughness = LoadImage2D(paths.roughness);
                textures.emission  = LoadImage2D(paths.emission);
                textures.height    = LoadImage2D(paths.height);
                
                String key = path + "#material/" + std::to_string(i);
                materials.push_back(LoadMaterial(key, desc, textures));
            }

            // 모델 생성
            ModelPtr created = Model::Create(std::move(result), std::move(materials));
            if (created && created->BuildMeshes())
            {
                glFinish();
                model = ModelPtr(std::move(created));
            }
        }

        bool success = static_cast<bool>(model);
        promise->set_value(std::move(model));
        return success;
    }

    void ResourceManager::Clear()
    {
        LockGuard<Mutex> lock(m_mutex);
        m_images2D.clear();
        m_imagesCube.clear();
        m_graphicsShaders.clear();
        m_computeShaders.clear();
        m_animations.clear();
        m_models.clear();
        m_materials.clear();
    }
}
