#pragma once
#include <Rendering/IRenderPipeline.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
	BDT_CLASS_PTR(MySimpleBus)
	BDT_CLASS_PTR(MySimpleShadowPass)
	BDT_CLASS_PTR(MySimpleGeometryPass)
	BDT_CLASS_PTR(MySimpleDeferredShadingPass)
	BDT_CLASS_PTR(MySimpleSkyboxPass)
	BDT_CLASS_PTR(MySimplePostProcessingPass)
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
		MySimpleShadowPassUPtr m_shadowPass;
		MySimpleGeometryPassUPtr m_geometryPass;
		MySimpleDeferredShadingPassUPtr m_deferredShadingPass;
		MySimpleSkyboxPassUPtr m_skyboxPass;
		MySimplePostProcessingPassUPtr m_postProcessingPass;
	};
}
