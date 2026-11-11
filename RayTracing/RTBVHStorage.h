#pragma once
#include "BLAS.h"
#include "TLAS.h"

namespace bdt
{
	BDT_CLASS_PTR(BLAS)
	BDT_CLASS_PTR(TLAS)
	BDT_CLASS_PTR(GLStorageBuffer)
	BDT_CLASS_PTR(RTBVHStorage)

	struct RTBVHMeshInput
	{
		uint32 meshIndex{ 0 };
		const List<vec3>* vertices{ nullptr };
		const List<uint32>* indices{ nullptr };
	};

	class RTBVHStorage
	{
		BDT_DISABLE_COPY(RTBVHStorage)
		BDT_DISABLE_MOVE(RTBVHStorage)

	public:
		~RTBVHStorage();
		static RTBVHStorageUPtr Create(EBVHBuildMode buildMode);

		bool Rebuild
		(
			const List<RTBVHMeshInput>& meshes,
			const List<GPUBVHNodeBounds>& instanceBounds,
			uint32 blasMaxLeafPrimitives = 4,
			uint32 tlasMaxLeafPrimitives = 1
		);
		bool Upload();
		void Bind() const;
		void Clear();
		void Shutdown();

		const BLAS* GetBLAS(uint32 meshIndex) const;
		const TLAS* GetTLAS() const;
		uint32 GetTLASDescriptorIndex() const;
		bool IsStorageDirty() const;

	private:
		RTBVHStorage();
		bool Init(EBVHBuildMode buildMode);
		bool AppendBVH(const BVH& bvh, uint32 meshIndex);
		bool UploadBuffers();

		List<BLASUPtr> m_blas;
		TLASUPtr m_tlas{ nullptr };
		uint32 m_tlasDescIdx{ 0xffffffffu };
		bool m_isStorageDirty{ false };
		EBVHBuildMode m_buildMode;

		List<GPUBVHNodeTopology> m_topology;
		List<GPUBVHNodeBounds> m_bounds;
		List<uint32> m_primitiveIndices;
		List<GPUBVHDescriptor> m_descriptors;

		GLStorageBufferUPtr m_topologyBuffer;
		GLStorageBufferUPtr m_boundsBuffer;
		GLStorageBufferUPtr m_primitiveBuffer;
		GLStorageBufferUPtr m_descriptorBuffer;
	};
}

