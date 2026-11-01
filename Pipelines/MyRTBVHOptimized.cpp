#include "pch.h"
#include "MyRTBVHOptimized.h"

namespace bdt
{
	MyRTBVHOptimized::MyRTBVHOptimized() : IRenderPipeline("MyRTBVHOptimized") { }

	bool MyRTBVHOptimized::Init()
	{
		return false;
	}

	bool MyRTBVHOptimized::Resize(uint32 width, uint32 height)
	{
		return false;
	}

	void MyRTBVHOptimized::Render(const RenderScene& scene, uint32 width, uint32 height)
	{
	}

	void MyRTBVHOptimized::Shutdown()
	{
	}
}