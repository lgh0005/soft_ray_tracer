#include "pch.h"
#include "RenderManager.h"
#include "IRenderPipeline.h"
#include <Core/GLVertexArray.h>
#include <Core/GLSampler.h>
#include <Core/GLVertices.h>
#include <Resources/ScreenMesh.h>

namespace bdt
{
	RenderManager::RenderManager() = default;
	RenderManager::~RenderManager() = default;

	bool RenderManager::Init()
	{
		// 0. OpenGL GPU 제한 인자 설정
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &m_maxColorAttachments);
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &m_maxDrawBuffers);
		if (m_maxTextureSize <= 0 || m_maxColorAttachments <= 0 || m_maxDrawBuffers <= 0)
			return false;

		// 1. 공유 VAO 생성
		m_staticVAO = GLVertexArray::Create();
		m_skinnedVAO = GLVertexArray::Create();
		m_screenVAO = GLVertexArray::Create();
		if (!m_staticVAO || !m_skinnedVAO || !m_screenVAO) return false;

		// 정점 Attribute 최초 설정
		SetStaticVAOAttrib();
		SetSkinnedVAOAttrib();
		SetScreenVAOAttrib();

		// 2. 공유 ScreenMesh 생성
		m_screenMesh = ScreenMesh::Create();
		if (!m_screenMesh) return false;

		// 3. 샘플러 생성
		m_linearClampSampler = GLSampler::Create();
		if (!m_linearClampSampler) return false;
		m_linearClampSampler->SetFilter(GL_LINEAR, GL_LINEAR);
		m_linearClampSampler->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		m_nearestClampSampler = GLSampler::Create();
		if (!m_nearestClampSampler) return false;
		m_nearestClampSampler->SetFilter(GL_NEAREST, GL_NEAREST);
		m_nearestClampSampler->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		m_linearRepeatSampler = GLSampler::Create();
		if (!m_linearRepeatSampler) return false;
		m_linearRepeatSampler->SetFilter(GL_LINEAR, GL_LINEAR);
		m_linearRepeatSampler->SetWrap(GL_REPEAT, GL_REPEAT);

		return true;
	}

	void RenderManager::Render(const RenderScene& scene, uint32 width, uint32 height)
	{
		if (width == 0 || height == 0)
			return;

		// 예약된 렌더 파이프라인 전환
		ApplyPipelineChange();

		// 현재 렌더 파이프라인 실행
		glClearColor(0.0f, 0.2f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (m_currentPipeline)
			m_currentPipeline->Render(scene, width, height);
	}

	void RenderManager::Shutdown()
	{
		// 전환 대기 상태 해제
		m_pendingPipeline = nullptr;

		// 현재 파이프라인 정리
		if (m_currentPipeline)
		{
			m_currentPipeline->Shutdown();
			m_currentPipeline = nullptr;
		}

		// 등록된 파이프라인 파괴
		m_pipelines.clear();

		// 공유 리소스 해제
		m_screenMesh.reset();

		m_staticVAO.reset();
		m_skinnedVAO.reset();
		m_screenVAO.reset();

		m_linearClampSampler.reset();
		m_nearestClampSampler.reset();
		m_linearRepeatSampler.reset();
	}

	bool RenderManager::Resize(uint32 width, uint32 height)
	{
		// 최소화되었거나 유효하지 않은 크기
		if (width == 0 || height == 0) return false;

		// 활성화된 렌더 파이프라인이 없는 경우
		if (!m_currentPipeline) return false;

		// 실제 렌더 타깃 크기 변경
		if (!m_currentPipeline->Resize(width, height))
			return false;

		return true;
	}

	void RenderManager::RegisterPipeline(IRenderPipelineUPtr pipeline)
	{
		if (!pipeline) return;
		const String tag = pipeline->GetTag();
		if (m_pipelines.contains(tag)) return;
		m_pipelines.emplace(tag, std::move(pipeline));
	}

	void RenderManager::ChangePipeline(const String& tag)
	{
		IRenderPipeline* nextPipeline = GetPipeline(tag);

		// 존재하지 않는 파이프라인
		if (!nextPipeline) return;

		// 이미 활성화된 파이프라인
		if (nextPipeline == m_currentPipeline)
		{
			m_pendingPipeline = nullptr;
			return;
		}

		// 다음 Render()에서 전환
		m_pendingPipeline = nextPipeline;
	}

	IRenderPipeline* RenderManager::GetCurrentPipeline() const
	{
		return m_currentPipeline;
	}

	IRenderPipeline* RenderManager::GetPipeline(const String& tag) const
	{
		auto it = m_pipelines.find(tag);
		if (it == m_pipelines.end()) return nullptr;
		return it->second.get();
	}

	void RenderManager::ApplyPipelineChange()
	{
		// 예약된 파이프라인이 없다면 전환하지 않음
		if (!m_pendingPipeline)
			return;

		IRenderPipeline* nextPipeline = m_pendingPipeline;
		m_pendingPipeline = nullptr;

		// 새 파이프라인 초기화
		if (!nextPipeline->Init())
		{
			nextPipeline->Shutdown();
			return;
		}

		// 기존 파이프라인 정리
		if (m_currentPipeline)
			m_currentPipeline->Shutdown();

		// 새 파이프라인 활성화
		m_currentPipeline = nextPipeline;
	}

	GLint RenderManager::GetMaxTextureSize() const
	{
		return m_maxTextureSize;
	}

	GLint RenderManager::GetMaxColorAttachments() const
	{
		return m_maxColorAttachments;
	}

	GLint RenderManager::GetMaxDrawBuffers() const
	{
		return m_maxDrawBuffers;
	}

	GLVertexArray* RenderManager::GetStaticVAO() const
	{
		return m_staticVAO.get();
	}

	GLVertexArray* RenderManager::GetSkinnedVAO() const
	{
		return m_skinnedVAO.get();
	}

	GLVertexArray* RenderManager::GetScreenVAO() const
	{
		return m_screenVAO.get();
	}

	ScreenMeshPtr RenderManager::GetScreenMesh() const
	{
		return m_screenMesh;
	}

	GLSampler* RenderManager::GetLinearClampSampler() const
	{
		return m_linearClampSampler.get();
	}

	GLSampler* RenderManager::GetNearestClampSampler() const
	{
		return m_nearestClampSampler.get();
	}

	GLSampler* RenderManager::GetLinearRepeatSampler() const
	{
		return m_linearRepeatSampler.get();
	}

	void RenderManager::SetScreenVAOAttrib()
	{
		m_screenVAO->SetAttrib(0, 0, 3, GL_FLOAT, false, offsetof(GLScreenVertex, position));
		m_screenVAO->SetAttrib(1, 0, 2, GL_FLOAT, false, offsetof(GLScreenVertex, texCoord));
	}

	void RenderManager::SetStaticVAOAttrib()
	{
		m_staticVAO->SetAttrib(0, 0, 3, GL_FLOAT, false, offsetof(GLStaticVertex, position));
		m_staticVAO->SetAttrib(1, 0, 3, GL_FLOAT, false, offsetof(GLStaticVertex, normal));
		m_staticVAO->SetAttrib(2, 0, 2, GL_FLOAT, false, offsetof(GLStaticVertex, texCoord));
		m_staticVAO->SetAttrib(3, 0, 3, GL_FLOAT, false, offsetof(GLStaticVertex, tangent));
	}

	void RenderManager::SetSkinnedVAOAttrib()
	{
		m_skinnedVAO->SetAttrib(0, 0, 3, GL_FLOAT, false, offsetof(GLSkinnedVertex, position));
		m_skinnedVAO->SetAttrib(1, 0, 3, GL_FLOAT, false, offsetof(GLSkinnedVertex, normal));
		m_skinnedVAO->SetAttrib(2, 0, 2, GL_FLOAT, false, offsetof(GLSkinnedVertex, texCoord));
		m_skinnedVAO->SetAttrib(3, 0, 3, GL_FLOAT, false, offsetof(GLSkinnedVertex, tangent));
		m_skinnedVAO->SetAttribI(4, 0, 4, GL_INT, offsetof(GLSkinnedVertex, boneIDs));
		m_skinnedVAO->SetAttrib(5, 0, 4, GL_FLOAT, false, offsetof(GLSkinnedVertex, weights));
	}
}
