#pragma once
#include <Rendering/IRenderPass.h>

namespace bdt
{
	BDT_CLASS_PTR(GraphicsShader)
	BDT_CLASS_PTR(MySimpleSSAOPass)

	class MySimpleSSAOPass : public IRenderPass
	{
		BDT_DISABLE_COPY(MySimpleSSAOPass)
		BDT_DISABLE_MOVE(MySimpleSSAOPass)
 
	public:
		virtual ~MySimpleSSAOPass() override;
		static MySimpleSSAOPassUPtr Create();
		
		bool Execute(IRenderBus& bus) override;
		void Shutdown() override;
		void SetRadius(float radius);
		void SetBias(float bias);
 
	private:
		MySimpleSSAOPass();
		virtual bool Init() override;

		ResourceHandle<GraphicsShader> m_shader;
		ResourceHandle<GraphicsShader> m_blurShader;
		RenderTexture2DUPtr m_noise;
		Array<vec3, 64> m_kernel{};

		float m_radius{ 0.5f };
		float m_bias{ 0.025f };
	};
}
