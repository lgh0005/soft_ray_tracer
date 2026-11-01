//#pragma once
//#include <Rendering/IRenderPass.h>
//#include <Resources/ResourceHandle.h>
//#include <Rendering/RenderScreen.h>
//#include "MySimplePassResources.h"
//
//namespace bdt
//{
//	BDT_CLASS_PTR(GraphicsShader)
//	BDT_CLASS_PTR(MySimpleSSAOPass)
//	class MySimpleSSAOPass : public IRenderPass
//	{
//		BDT_DISABLE_COPY(MySimpleSSAOPass)
//		BDT_DISABLE_MOVE(MySimpleSSAOPass)
//	public:
//		MySimpleSSAOPass() = default;
//		~MySimpleSSAOPass() override;
//		static MySimpleSSAOPassUPtr Create();
//		bool Init() override;
//		void Execute(IRenderBus& bus) override;
//		void Shutdown() override;
//		void SetRadius(float radius);
//		void SetBias(float bias);
//	private:
//		ResourceHandle<GraphicsShader> m_shader;
//		ResourceHandle<GraphicsShader> m_blurShader;
//		MySimpleColorTarget m_raw;
//		RenderTexture2DUPtr m_noise;
//		Array<vec3, 64> m_kernel{};
//		RenderScreen m_screen;
//		float m_radius{ 0.5f };
//		float m_bias{ 0.025f };
//		bool m_initialized{ false };
//	};
//}
