#pragma once
#include <Rendering/IRenderPipeline.h>
#include "MySimpleBus.h"
#include "MySimpleGeometryPass.h"
#include "MySimpleOutputPass.h"

namespace bdt
{
	BDT_CLASS_PTR(MySimpleBus)
	BDT_CLASS_PTR(MySimpleGeometryPass)
	BDT_CLASS_PTR(MySimpleOutputPass)
	BDT_CLASS_PTR(MySimplePipeline)

	class MySimplePipeline : public IRenderPipeline
	{
		BDT_DISABLE_COPY(MySimplePipeline)
		BDT_DISABLE_MOVE(MySimplePipeline)
		using Super = IRenderPipeline;

	public:
		virtual ~MySimplePipeline() override;
		static MySimplePipelineUPtr Create();
		virtual bool Resize(uint32 width, uint32 height);
		virtual void Render(const RenderScene& scene, uint32 width, uint32 height) override;
		virtual void Shutdown() override;

	private:
		MySimplePipeline();
		virtual bool Init() override;

		MySimpleBusUPtr m_bus;
		MySimpleGeometryPassUPtr m_geometryPass;
		MySimpleOutputPassUPtr m_outputPass;
	};
}
