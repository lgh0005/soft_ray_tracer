#pragma once
#include <Rendering/IRenderPass.h>

namespace bdt
{
	BDT_CLASS_PTR(GraphicsShader)
	BDT_CLASS_PTR(MySimpleShadowPass)

	class MySimpleShadowPass : public IRenderPass
	{
		BDT_DISABLE_COPY(MySimpleShadowPass)
		BDT_DISABLE_MOVE(MySimpleShadowPass)

	public:
		virtual ~MySimpleShadowPass() override;
		static MySimpleShadowPassUPtr Create(uint32 resolution = 1024);
		virtual void Execute(IRenderBus& bus) override;
		virtual void Shutdown() override;

	private:
		explicit MySimpleShadowPass(uint32 resolution);
		virtual bool Init() override;

		uint32 m_resolution{ 1024 };
		ResourceHandle<GraphicsShader> m_shadowShaderStatic;
		ResourceHandle<GraphicsShader> m_shadowShaderSkinned;
	};
}
