#pragma once
#include "ModelData.h"
#include "Material.h"
#include <variant>

namespace bdt
{
	BDT_CLASS_PTR(Model)
	BDT_CLASS_PTR(Skeleton)
	BDT_CLASS_PTR(StaticMesh)
	BDT_CLASS_PTR(SkinnedMesh)

	class Model
	{
		BDT_DISABLE_COPY(Model)
		BDT_DISABLE_MOVE(Model)
		using ModelMeshResource = std::variant
		<
			std::monostate,
			StaticMeshPtr,
			SkinnedMeshPtr
		>;

	public:
		~Model();
		static ModelPtr Create
		(
			ModelImportResult&& result,
			List<ResourceHandle<Material>> materials
		);
		bool BuildMeshes();

		const ModelData& GetData() const;
		const Skeleton* GetSkeleton() const;
		const List<ModelMeshResource>& GetMeshes() const;
		const List<ResourceHandle<Material>>& GetMaterials() const;

	private:
		Model();
		bool Init
		(
			ModelImportResult&& result,
			List<ResourceHandle<Material>> materials
		);

	private:
		ModelData m_data;
		SkeletonUPtr m_skeleton{ nullptr };
		List<ModelMeshResource> m_meshes;
		List<ResourceHandle<Material>> m_materials;
	};
}

