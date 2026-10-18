#pragma once
#include "ResourceHandle.h"
#include "Model.h"
#include "ModelImporter.h"
#include "Animation.h"
#include "Image2D.h"
#include "ImageCube.h"
#include "GraphicsShader.h"
#include "ComputeShader.h"
#include "Material.h"

namespace bdt
{
	class ResourceManager
	{
		BDT_DECLARE_SINGLE(ResourceManager)
		template<typename T> using Cache = Dictionary<String, ResourceHandle<T>>;

    public:
        ResourceHandle<Image2D>        LoadImage2D(const String& path);
        ResourceHandle<ImageCube>      LoadImageCube(const String& path);
        ResourceHandle<GraphicsShader> LoadGraphicsShader(const String& key, List<ShaderFile> files);
        ResourceHandle<ComputeShader>  LoadComputeShader(const String& key, List<ShaderFile> files);
        ResourceHandle<Animation>      LoadAnimation(const String& path);
        ResourceHandle<Model>          LoadModel(const String& path);
        ResourceHandle<Material>       LoadMaterial(const String& key, const MaterialDesc& desc, const MaterialTextures& textures);

        // TaskManager 작업 종료 후 GL 컨텍스트가 살아 있을 때 호출
        void Clear();

    private:
        void ExtractModel
        (
            const String& path,
            const std::shared_ptr<ModelImporter>& importer,
            const std::shared_ptr<ModelImportParts>& parts,
            const std::shared_ptr<Promise<ModelPtr>>& promise
        );

        void QueueModelBuild
        (
            const String& path,
            const std::shared_ptr<ModelImportParts>& parts,
            const std::shared_ptr<Promise<ModelPtr>>& promise
        );

        bool BuildModel
        (
            const String& path,
            const std::shared_ptr<ModelImportParts>& parts,
            const std::shared_ptr<Promise<ModelPtr>>& promise
        );

	private:
        template<typename T>
        ResourceHandle<T> Reserve
        (
            const String& key,
            Cache<T>& cache,
            std::shared_ptr<Promise<std::shared_ptr<T>>>& promise
        );

        template<typename T, typename Factory>
        ResourceHandle<T> Load
        (
            const String& key,
            Cache<T>& cache,
            Factory factory,
            bool gpu
        );

    private:
        Mutex m_mutex;

        Cache<Image2D> m_images2D;
        Cache<ImageCube> m_imagesCube;
        Cache<GraphicsShader> m_graphicsShaders;
        Cache<ComputeShader> m_computeShaders;
        Cache<Animation> m_animations;
        Cache<Model> m_models;
        Cache<Material> m_materials;
	};
}

#include "ResourceManager.inl"