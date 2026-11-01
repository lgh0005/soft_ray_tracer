#pragma once
#include <Rendering\IRenderPipeline.h>

namespace bdt
{
	class MYRTSpatialOptimized : public IRenderPipeline
	{
	public:
		MYRTSpatialOptimized();
		virtual bool Init() override;
		virtual bool Resize(uint32 width, uint32 height) override;
		virtual void Render(const RenderScene& scene, uint32 width, uint32 height) override;
		virtual void Shutdown() override;
	};
}
