#pragma once
#include <Rendering/IRenderBus.h>

namespace bdt
{
	BDT_CLASS_PTR(GLBuffer)
	BDT_CLASS_PTR(GLStorageBuffer)
	BDT_CLASS_PTR(GraphicsShader)
	BDT_CLASS_PTR(MySimpleGBuffer)
	BDT_CLASS_PTR(StaticRenderQueue)
	BDT_CLASS_PTR(SkinnedRenderQueue)
	BDT_CLASS_PTR(RenderTexture2D)
	BDT_CLASS_PTR(RenderTextureArray2D)
	BDT_CLASS_PTR(RenderScreen)
	BDT_CLASS_PTR(MySimpleBus)

	class MySimpleBus : public IRenderBus
	{
		BDT_DISABLE_COPY(MySimpleBus)
		BDT_DISABLE_MOVE(MySimpleBus)

	public:
		virtual ~MySimpleBus() override;
		static MySimpleBusUPtr Create();
		bool BeginFrame(const RenderScene& scene, uint32 width, uint32 height);
		virtual bool Resize(uint32 width, uint32 height) override;
		void Shutdown();

	public:
		MySimpleGBuffer* GetGBuffer() const;
		void SubmitObjectToRenderQueue(const RenderObject& object);
		
		// For Geometry Pass
		void ExecuteRenderQueues
		(
			const GraphicsShaderPtr& staticShader,
			const GraphicsShaderPtr& skinnedShader
		);

		// For Shadow Pass
		void ExecuteShadowRenderQueues
		(
			const GraphicsShaderPtr& staticShader,
			const GraphicsShaderPtr& skinnedShader
		);

		// For Post-Processing Pass
		Framebuffer2D* GetLDRFramebuffer() const;
		RenderTexture2D* GetLDRTexture() const;

		bool PrepareShadowTarget(uint32 resolution, uint32 layers);
		Framebuffer2D* GetShadowFramebuffer() const;
		RenderTextureArray2D* GetShadowTextureArray() const;
		RenderScreen* GetRenderScreen() const;

		void UpdateSkyboxData(const SkyboxData& data);
		void SetSkybox(const RenderTextureCubePtr& texture) { m_skyboxTexture = texture; }
		const RenderTextureCubePtr& GetSkybox() const { return m_skyboxTexture; }

	private:
		MySimpleBus();
		bool Init();

		// Shared resource for render pipeline
		MySimpleGBufferUPtr m_gBuffer{ nullptr };
		GLBufferUPtr m_cameraBuffer{ nullptr };
		GLBufferUPtr m_skyboxBuffer{ nullptr };
		GLStorageBufferUPtr m_lightBuffer{ nullptr };
		GLStorageBufferUPtr m_shadowBuffer{ nullptr };

		// Skybox data for render pipeline
		RenderTextureCubePtr m_skyboxTexture;

		// Shadow data for render pipeline
		Framebuffer2DUPtr m_shadowFramebuffer;
		RenderTextureArray2DUPtr m_shadowTextureArray;

		// Post-Processing
		Framebuffer2DUPtr m_ldrFramebuffer;
		RenderTexture2DUPtr m_ldrTexture;

		// Shared render queues for render pipeline
		StaticRenderQueueUPtr m_staticQueue{ nullptr };
		SkinnedRenderQueueUPtr m_skinnedQueue{ nullptr };

		// Shared RenderScreen
		RenderScreenUPtr m_screen;
	};
}
