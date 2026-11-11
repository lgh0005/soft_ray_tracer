#pragma once
#include "BVH.h"

namespace bdt
{
	class TLAS final : public BVH
	{
		BDT_DISABLE_COPY(TLAS);
		BDT_DISABLE_MOVE(TLAS);
		using Super = BVH;

	public:
		explicit TLAS(EBVHBuildMode buildMode);
		virtual ~TLAS() override;

		bool Build
		(
			const List<GPUBVHNodeBounds>& instanceBounds,
			uint32 maxLeafPrimitives = 1
		);

		bool Rebuild
		(
			const List<GPUBVHNodeBounds>& instanceBounds,
			uint32 maxLeafPrimitives = 1
		);

		bool Refit(const List<GPUBVHNodeBounds>& instanceBounds);

	private:
		bool ValidateInstanceBounds(const List<GPUBVHNodeBounds>& instanceBounds) const;
	};
}
