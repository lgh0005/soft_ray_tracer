#pragma once
#include <glad/gl.h>
#include "RenderScene.h"

namespace bdt
{
	BDT_CLASS_PTR(GLVertexArray)
	BDT_CLASS_PTR(GLSampler)
	BDT_CLASS_PTR(ScreenMesh)
	BDT_CLASS_PTR(IRenderPipeline)

	class RenderManager
	{
		BDT_DECLARE_SINGLE(RenderManager)

	public:
		bool Init();
		void Render(const RenderScene& scene, uint32 width, uint32 height);
		void Shutdown();
		bool Resize(uint32 width, uint32 height);

		void RegisterPipeline(IRenderPipelineUPtr pipeline);
		void ChangePipeline(const String& tag);
		IRenderPipeline* GetCurrentPipeline() const;
		IRenderPipeline* GetPipeline(const String& tag) const;

	public:
		GLint GetMaxTextureSize() const;
		GLint GetMaxColorAttachments() const;
		GLint GetMaxDrawBuffers() const;
		GLVertexArray* GetStaticVAO() const;
		GLVertexArray* GetSkinnedVAO() const;
		GLVertexArray* GetScreenVAO() const;
		ScreenMeshPtr GetScreenMesh() const;
		GLSampler* GetLinearClampSampler() const;
		GLSampler* GetNearestClampSampler() const;
		GLSampler* GetLinearRepeatSampler() const;

	private:
		void ApplyPipelineChange();
		void SetStaticVAOAttrib();
		void SetSkinnedVAOAttrib();
		void SetScreenVAOAttrib();

	private:

		// OpenGL GPU 제한 인자
		GLint m_maxTextureSize{ 0 };
		GLint m_maxColorAttachments{ 0 };
		GLint m_maxDrawBuffers{ 0 };

		// 렌더 파이프라인 등록
		Dictionary<String, IRenderPipelineUPtr> m_pipelines;
		IRenderPipeline* m_currentPipeline{ nullptr };
		IRenderPipeline* m_pendingPipeline{ nullptr };

		// 공유 VAO
		GLVertexArrayUPtr m_staticVAO{ nullptr };
		GLVertexArrayUPtr m_skinnedVAO{ nullptr };
		GLVertexArrayUPtr m_screenVAO{ nullptr };

		// 공유 화면 메시
		ScreenMeshPtr m_screenMesh{ nullptr };

		// 공유 Sampler
		GLSamplerUPtr m_linearClampSampler{ nullptr };
		GLSamplerUPtr m_nearestClampSampler{ nullptr };
		GLSamplerUPtr m_linearRepeatSampler{ nullptr };
	};
}

