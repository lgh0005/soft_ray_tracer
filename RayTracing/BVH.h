#pragma once
#include "RTAccelerates.h"
#include "Enums.h"

namespace bdt
{
	class BVH
	{
		BDT_DISABLE_COPY(BVH);
		BDT_DISABLE_MOVE(BVH);

	public:
		explicit BVH(EBVHBuildMode buildMode);
		virtual ~BVH();

		// Data Access
		const List<GPUBVHNodeTopology>& GetTopology() const;
		const List<GPUBVHNodeBounds>& GetBounds() const;
		const List<uint32>& GetPrimitiveIndices() const;

		// Tree Information
		uint32 GetRootIndex() const;
		uint32 GetNodeCount() const;
		uint32 GetPrimitiveCount() const;

		bool IsEmpty() const;
		bool IsLeaf(uint32 nodeIndex) const;

		// Bounds Update
		void UpdateNodeBounds(uint32 nodeIndex, const GPUBVHNodeBounds& bounds);

		// Resource Management
		void Clear();

	protected:

		// Build 결과를 공통 BVH 데이터로 설정
		void SetData
		(
			List<GPUBVHNodeTopology>&& topology,
			List<GPUBVHNodeBounds>&& bounds,
			List<uint32>&& primitiveIndices,
			uint32 rootIndex
		);

		bool BuildFromBounds
		(
			const List<GPUBVHNodeBounds>& primitiveBounds,
			uint32 maxLeafPrimitives = 4
		);

	private:
		GPUBVHNodeBounds CalculateNodeBounds
		(
			const List<GPUBVHNodeBounds>& primitiveBounds,
			const List<uint32>& indices,
			uint32 begin,
			uint32 end
		) const;

		uint32 BuildNode
		(
			const List<GPUBVHNodeBounds>& primitiveBounds,
			List<GPUBVHNodeTopology>& topology,
			List<GPUBVHNodeBounds>& bounds,
			List<uint32>& indices,
			uint32 begin,
			uint32 end,
			uint32 parent,
			uint32 maxLeafPrimitives
		);

		// 긴축 기반 중앙값 분할 방식
		bool SplitMedian
		(
			const List<GPUBVHNodeBounds>& primitiveBounds,
			List<uint32>& indices,
			uint32 begin,
			uint32 end,
			uint32& splitIndex
		);

		// 표면적 휴리스틱 (SAH)
		float CalculateSurfaceArea(const GPUBVHNodeBounds& bounds) const;

		bool SplitSAH
		(
			const List<GPUBVHNodeBounds>& primitiveBounds,
			List<uint32>& indices,
			uint32 begin,
			uint32 end,
			const GPUBVHNodeBounds& nodeBounds,
			uint32& splitIndex
		);

	private:
		List<GPUBVHNodeTopology> m_topology;
		List<GPUBVHNodeBounds> m_bounds;
		List<uint32> m_primitiveIndices;
		uint32 m_rootIndex { 0xffffffffu };
		EBVHBuildMode m_buildMode{ EBVHBuildMode::MedianSplit };
	};
}
