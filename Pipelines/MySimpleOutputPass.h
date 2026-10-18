#pragma once
#include <Resources/ResourceHandle.h>
#include <Rendering/IRenderPass.h>
#include <Rendering/RenderScreen.h>

namespace bdt
{
	BDT_CLASS_PTR(GLVertexArray)
	BDT_CLASS_PTR(GLSampler)
	BDT_CLASS_PTR(GraphicsShader)

	BDT_CLASS_PTR(MySimpleOutputPass)

	class MySimpleOutputPass : public IRenderPass
	{
		BDT_DISABLE_COPY(MySimpleOutputPass)
		BDT_DISABLE_MOVE(MySimpleOutputPass)

	public:
		virtual ~MySimpleOutputPass() override;
		static MySimpleOutputPassUPtr Create();
		virtual void Execute(IRenderBus& bus) override;
		virtual void Shutdown() override;

	private:
		MySimpleOutputPass();
		virtual bool Init() override;

		ResourceHandle<GraphicsShader> m_outputShader;
		RenderScreen m_renderScreen;
	};
}
