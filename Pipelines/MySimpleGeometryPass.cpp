#include "pch.h"
#include "MyGBuffer.h"
#include "MySimpleGeometryPass.h"
#include "MySimpleBus.h"

#include <Core/GLSampler.h>
#include <Resources/Resource.h>
#include <Resources/GraphicsShader.h>
#include <Rendering/RenderScene.h>
#include <Rendering/RenderObject.h>
#include <Rendering/StaticRenderQueue.h>
#include <Rendering/SkinnedRenderQueue.h>
#include <Rendering/Instances.h>

namespace bdt
{
	MySimpleGeometryPass::MySimpleGeometryPass() = default;
	MySimpleGeometryPass::~MySimpleGeometryPass() = default;

	MySimpleGeometryPassUPtr MySimpleGeometryPass::Create()
	{
		MySimpleGeometryPassUPtr result(new MySimpleGeometryPass());
		if (!result->Init()) return nullptr;
		return result;
	}

	bool MySimpleGeometryPass::Init()
	{
		// RenderQueue 생성
		m_staticQueue = StaticRenderQueue::Create();
		m_skinnedQueue = SkinnedRenderQueue::Create();
		if (!m_staticQueue || !m_skinnedQueue)
			return false;

		// 프로그램 비동기 로드 요청
		m_geometryShaderStatic = BDT_RESOURCE.LoadGraphicsShader
		(
			"MyLocalGeometry.Static",
			{
				{ GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_geometry_pass_static.vert" },
				{ GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_geometry_pass.frag" }
			}
		);

		m_geometryShaderSkinned = BDT_RESOURCE.LoadGraphicsShader
		(
			"MyLocalGeometry.Skinned",
			{
				{ GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_geometry_pass_skinned.vert" },
				{ GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_geometry_pass.frag" }
			}
		);

		return true;
	}

	void MySimpleGeometryPass::Execute(IRenderBus& bus)
	{
		// 1. RenderQueue 초기화
		m_staticQueue->Clear();
		m_skinnedQueue->Clear();

		// 2. RenderBus 및 RenderScene 가져오기
		auto* geometryBus = static_cast<MySimpleBus*>(&bus);
		if (!geometryBus) return;

		const RenderScene* scene = geometryBus->GetRenderScene();
		MyGBuffer* gBuffer = geometryBus->GetGBuffer();

		// 3. 셰이더 준비 여부 확인
		auto staticShader = m_geometryShaderStatic.Get();
		auto skinnedShader = m_geometryShaderSkinned.Get();

		// 4. 렌더링 객체를 Queue에 등록
		for (const RenderObject* object : scene->GetObjects())
		{
			if (object)
				SubmitObject(*object);
		}

		for (const auto* group : scene->GetObjectGroups())
		{
			if (!group) continue;
			for (const RenderObject& object : *group)
				SubmitObject(object);
		}

		// 5. GBuffer 바인딩
		gBuffer->Bind();
		glViewport(0, 0, static_cast<GLsizei>(gBuffer->GetWidth()), static_cast<GLsizei>(gBuffer->GetHeight()));

		// 6. GeometryPass 렌더링 상태
		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_FRAMEBUFFER_SRGB);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		// 7. GBuffer 초기화
		const GLfloat clearColor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
		for (GLint i = 0; i < static_cast<GLint>(EGBufferTextureSlot::Depth); ++i)
			glClearBufferfv(GL_COLOR, i, clearColor);
		const GLfloat clearDepth = 1.0f; glClearBufferfv(GL_DEPTH, 0, &clearDepth);

		// 8. 샘플러 바인딩
		GLSampler* sampler = BDT_RENDER.GetLinearRepeatSampler();
		for (uint32 slot = 0; slot < static_cast<uint32>(EMaterialTextureSlot::Count); ++slot)
			glBindSampler(slot, sampler->GetSamplerID());

		// 9. Static Mesh 인스턴싱 렌더링
		if (staticShader && staticShader->IsValid()) 
			m_staticQueue->Execute(staticShader.get());

		// 10. Skinned Mesh 인스턴싱 렌더링
		if (skinnedShader && skinnedShader->IsValid()) 
			m_skinnedQueue->Execute(skinnedShader.get());

		// 11. Framebuffer 및 셰이더 바인딩 해제
		gBuffer->Unbind();
		GraphicsShader::Unbind();
	}

	void MySimpleGeometryPass::SubmitObject(const RenderObject& object)
	{
		const auto& mesh = object.GetMesh();
		const auto material = object.GetMaterial().Get();
		if (!mesh) return;

		if (object.IsSkinned())
		{
			const List<mat4>* bones = object.GetBoneMatrices();
			if (!bones) return;

			SkinnedInstanceProperty property{};
			property.worldMatrix = object.GetWorldMatrix();
			m_skinnedQueue->Add(mesh.get(), material.get(), property, *bones);
		}
		else
		{
			StaticInstanceProperty property{};
			property.worldMatrix = object.GetWorldMatrix();
			m_staticQueue->Add(mesh.get(), material.get(), property);
		}
	}

	void MySimpleGeometryPass::Shutdown()
	{
		m_staticQueue.reset();
		m_skinnedQueue.reset();
		m_geometryShaderStatic = {};
		m_geometryShaderSkinned = {};
	}
}
