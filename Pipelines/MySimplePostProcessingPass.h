#pragma once
#include <Rendering/IRenderPass.h>
#include <Rendering/Framebuffer2D.h>
#include <Rendering/RenderTexture2D.h>

namespace bdt
{
	BDT_CLASS_PTR(GraphicsShader)
	BDT_CLASS_PTR(RenderScreen)
	BDT_CLASS_PTR(MySimplePostProcessingPass)
 
	class MySimplePostProcessingPass : public IRenderPass
	{
		BDT_DISABLE_COPY(MySimplePostProcessingPass)
		BDT_DISABLE_MOVE(MySimplePostProcessingPass)

	public:
		virtual ~MySimplePostProcessingPass() override;
		static MySimplePostProcessingPassUPtr Create();
		void Execute(IRenderBus& bus) override;
		void Shutdown() override;

		void SetBloomThreshold(float threshold);
		void SetBloomStrength(float strength);
		RenderTexture2D* GetBloomTexture() const;

	private:
		MySimplePostProcessingPass() = default;
		bool Init() override;
		bool PrepareBloomTargets(uint32 width, uint32 height);
		void RenderBloom(RenderTexture2D* source, RenderScreen* screen, GraphicsShader* shader);

		// TODO : Bloom 패스를 따로 빼는 것도 고려
		struct BloomLevel
		{
			RenderTexture2DUPtr texture;
			Framebuffer2DUPtr framebuffer;
		};

		List<BloomLevel> m_bloomLevels;
		uint32 m_bloomWidth{ 0 };
		uint32 m_bloomHeight{ 0 };
		bool m_bloomReady{ false };

		GLSamplerUPtr m_sampler;
		ResourceHandle<GraphicsShader> m_toneMappingShader;
		ResourceHandle<GraphicsShader> m_fxaaShader;
		ResourceHandle<GraphicsShader> m_bloomShader;

		float m_exposure{ 1.0f };
		float m_gamma{ 2.2f };
		float m_bloomThreshold{ 1.0f };
		float m_bloomStrength{ 0.04f };
	};
}
