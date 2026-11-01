#pragma once
#include <Rendering/IRenderPass.h>

namespace bdt
{
	BDT_CLASS_PTR(GLVertexArray)
	BDT_CLASS_PTR(GLSampler)
	BDT_CLASS_PTR(GraphicsShader)

	BDT_CLASS_PTR(MySimpleDeferredShadingPass)

	class MySimpleDeferredShadingPass : public IRenderPass
	{
		BDT_DISABLE_COPY(MySimpleDeferredShadingPass)
		BDT_DISABLE_MOVE(MySimpleDeferredShadingPass)

	public:
		virtual ~MySimpleDeferredShadingPass() override;
		static MySimpleDeferredShadingPassUPtr Create();
		virtual void Execute(IRenderBus& bus) override;
		virtual void Shutdown() override;

	private:
		MySimpleDeferredShadingPass();
		virtual bool Init() override;

		ResourceHandle<GraphicsShader> m_outputShader;
		GLSamplerUPtr m_shadowSampler;
		GLSamplerUPtr m_linearSampler;
		GLSamplerUPtr m_prefilterSampler;
	};
}
