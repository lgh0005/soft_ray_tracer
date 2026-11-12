#pragma once
#include "Enums.h"
#include "RTAccelerates.h"
#include "RTBVHStorage.h"

namespace bdt
{
	BDT_CLASS_PTR(RTBVHBuilder)

	class RTBVHBuilder
	{
		BDT_DISABLE_COPY(RTBVHBuilder)
		BDT_DISABLE_MOVE(RTBVHBuilder)

	public:
		~RTBVHBuilder();
		static RTBVHBuilderUPtr Create(EBVHBuildMode buildMode);
		bool Prepare();
		void Shutdown();

		RTBVHStorage* GetStorage() const;
		void MarkBLASDirty();
		void MarkTLASDirty();
		void MarkTLASRefitDirty();

	private:
		RTBVHBuilder();
		bool Init(EBVHBuildMode buildMode);

		bool PrepareMeshInputs();
		bool PrepareInstanceBounds();
		GPUBVHNodeBounds TransformBounds
		(
			const GPUBVHNodeBounds& localBounds,
			const mat4& worldMatrix
		) const;

	private:

		// BVH Storage
		RTBVHStorageUPtr m_storage{ nullptr };

		// BVH Dirty Flag
		bool m_blasDirty		{ true };
		bool m_tlasDirty		{ true };
		bool m_tlasRefitDirty	{ false };

		// Mesh별 CPU 데이터 캐시
		List<List<vec3>> m_vertices;
		List<List<uint32>> m_indices;
		List<GPUBVHNodeBounds> m_meshLocalBounds;

		// BLAS / TLAS 구축 입력
		List<RTBVHMeshInput> m_meshInputs;
		List<GPUBVHNodeBounds> m_instanceBounds;
	};
}

