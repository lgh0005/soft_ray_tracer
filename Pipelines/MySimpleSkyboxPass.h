#pragma once
#include <Rendering/IRenderPass.h>

namespace bdt
{
	BDT_CLASS_PTR(GraphicsShader)
	BDT_CLASS_PTR(Image2D)
	BDT_CLASS_PTR(ImageBasedLight)
	BDT_CLASS_PTR(GLBuffer)
	BDT_CLASS_PTR(GLSampler)
	BDT_CLASS_PTR(MySimpleSkyboxPass)

	class MySimpleSkyboxPass : public IRenderPass
	{
		BDT_DISABLE_COPY(MySimpleSkyboxPass)
		BDT_DISABLE_MOVE(MySimpleSkyboxPass)

	public:
		virtual ~MySimpleSkyboxPass() override;
		static MySimpleSkyboxPassUPtr Create();
		void Execute(IRenderBus& bus) override;
		void Shutdown() override;

	private:
		MySimpleSkyboxPass();
		virtual bool Init() override;

		ResourceHandle<GraphicsShader> m_shader;
		GLSamplerUPtr m_skyboxSampler;
	};
}
