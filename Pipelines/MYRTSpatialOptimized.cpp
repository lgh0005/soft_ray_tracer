#include "pch.h"
#include "MYRTSpatialOptimized.h"

namespace bdt
{
	MYRTSpatialOptimized::MYRTSpatialOptimized() : IRenderPipeline("MyRTBVHOptimized") {}

	bool MYRTSpatialOptimized::Init()
	{
		return false;
	}

	bool MYRTSpatialOptimized::Resize(uint32 width, uint32 height)
	{
		return false;
	}

	void MYRTSpatialOptimized::Render(const RenderScene& scene, uint32 width, uint32 height)
	{}

	void MYRTSpatialOptimized::Shutdown()
	{}
}