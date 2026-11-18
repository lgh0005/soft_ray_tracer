#pragma once
#include "BLAS.h"
#include "TLAS.h"
#include "RTInformations.h"

namespace bdt
{
	BDT_CLASS_PTR(BLAS)
	BDT_CLASS_PTR(TLAS)
	BDT_CLASS_PTR(GLStorageBuffer)
	BDT_CLASS_PTR(RTBVHStorage)

	struct RTBVHRefitData
	{
		uint32 instanceCount{ 0 };
		uint32 maxSkinnedNodeCount{ 0 };
		uint32 tlasNodeCount{ 0 };
		bool refitBLAS{ false };
		bool refitTLAS{ false };
	};

	class RTBVHStorage
	{
		BDT_DISABLE_COPY(RTBVHStorage)
		BDT_DISABLE_MOVE(RTBVHStorage)

	public:
		~RTBVHStorage();
		static RTBVHStorageUPtr Create(EBVHBuildMode buildMode);
		bool Upload();
		void Bind() const;
		void Shutdown();

	/*==========================================//
	//             Clear Operations             //
	//==========================================*/
	public:
		void Clear();
		bool ClearBLAS();
		bool ClearTLAS();

	/*==========================================//
	//               Data Access                //
	//==========================================*/
	public:
		const BLAS* GetBLAS(uint32 meshIndex) const;
		const TLAS* GetTLAS() const;
		uint32 GetTLASDescriptorIndex() const;
		uint64 GetBLASRevision() const;
		bool IsStorageDirty() const;

	/*==========================================//
	//           BLAS Refit Level Tasks          //
	//==========================================*/
	public:
		bool PrepareRefit(RTBVHRefitData& data, bool parallelInternalRefit);
		bool PrepareRefitLevelTasks();
		const List<uvec2>& GetRefitLevelRanges() const;
		void BindRefitLevelTasks() const;

	/*==========================================//
	//               BVH Updates                //
	//==========================================*/
	public:
		// Configures subsequent rebuilds. Use RTBVHBuilder to invalidate live trees.
		bool SetBuildSettings(const RTBVHBuildSettings& settings);
		const RTBVHBuildSettings& GetBuildSettings() const;
		bool RebuildBLAS(const List<RTBVHMeshInput>& meshes);
		bool RebuildBLAS(const List<RTBVHMeshInput>& meshes, uint32 maxLeafPrimitives);
		bool RebuildTLAS(const List<GPUBVHNodeBounds>& instanceBounds);
		bool RebuildTLAS(const List<GPUBVHNodeBounds>& instanceBounds, uint32 maxLeafPrimitives);
		bool RefitTLAS(const List<GPUBVHNodeBounds>& instanceBounds);
		bool ResizeSkinnedBounds(uint32 nodeCount);

	private:
		RTBVHStorage();
		bool Init(EBVHBuildMode buildMode);

	/*==========================================//
	//           Packing / Upload Helpers       //
	//==========================================*/
		bool AppendBVH(const BVH& bvh, uint32 meshIndex);
		bool UploadBounds();
		bool UploadBuffers();

	private:
		bool m_isStorageDirty{ false };
		bool m_isBoundsDirty{ false };

		List<BLASUPtr> m_blas;
		TLASUPtr m_tlas{ nullptr };
		uint32 m_tlasDescIdx{ 0xffffffffu };
		RTBVHBuildSettings m_buildSettings;
		uint64 m_blasRevision{ 0 };

		// Schedule data follows the BLAS lifetime
		GLStorageBufferUPtr m_levelTaskBuffer;
		List<uvec2> m_levelRanges;
		bool m_isLevelTasksDirty{ true };

		List<GPUBVHNodeTopology> m_topology;
		List<GPUBVHNodeBounds> m_bounds;
		List<uint32> m_primitiveIndices;
		List<GPUBVHDescriptor> m_descriptors;

		GLStorageBufferUPtr m_topologyBuffer	  { nullptr };
		GLStorageBufferUPtr m_boundsBuffer		  { nullptr };
		GLStorageBufferUPtr m_primitiveBuffer	  { nullptr };
		GLStorageBufferUPtr m_descriptorBuffer	  { nullptr };
		GLStorageBufferUPtr m_skinnedBoundsBuffer { nullptr };
	};
}

