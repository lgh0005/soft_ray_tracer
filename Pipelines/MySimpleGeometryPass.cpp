#include "pch.h"
#include "MySimpleGeometryPass.h"
#include "MySimpleGBuffer.h"
#include "MySimpleBus.h"

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
		// Material Sampler 생성
		m_materialSampler = GLSampler::Create();
		if (!m_materialSampler) return false;
		m_materialSampler->SetFilter(GL_LINEAR, GL_LINEAR);
		m_materialSampler->SetWrap(GL_REPEAT, GL_REPEAT);

		// GeometryShaderStatic 로드
		m_geometryShaderStatic = BDT_RESOURCE.LoadGraphicsShader
		(
			"MyLocalGeometry.Static",
			{
				{ GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_geometry_pass_static.vert" },
				{ GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_geometry_pass.frag" }
			}
		);

		// GeometryShaderSkinned 로드
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
		// 1. RenderBus 및 RenderScene 가져오기
		auto* geometryBus = static_cast<MySimpleBus*>(&bus);
		if (!geometryBus) return;

		const RenderScene* scene = geometryBus->GetRenderScene();
		MySimpleGBuffer* gBuffer = geometryBus->GetGBuffer();
		if (!scene || !gBuffer) return;

		// 2. 셰이더 준비 여부 확인
		auto staticShader = m_geometryShaderStatic.Get();
		auto skinnedShader = m_geometryShaderSkinned.Get();

		// 3. GBuffer 바인딩
		gBuffer->Bind();
		glViewport(0, 0, (GLsizei)gBuffer->GetWidth(), (GLsizei)gBuffer->GetHeight());

		// 4. GeometryPass 렌더링 상태
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		// 5. GBuffer 초기화
		gBuffer->Clear();

		// 6. 샘플러 바인딩
		for (uint32 slot = 0; slot < (uint32)EMaterialTextureSlot::Count; ++slot)
			m_materialSampler->Bind(slot);

		// 7. 인스턴싱 렌더링
		geometryBus->ExecuteRenderQueues(staticShader, skinnedShader);

		// 8. Framebuffer 및 셰이더 바인딩 해제
		gBuffer->Unbind();
		for (uint32 slot = 0; slot < (uint32)EMaterialTextureSlot::Count; ++slot)
			GLSampler::Unbind(slot);
		GraphicsShader::Unbind();
	}

	void MySimpleGeometryPass::Shutdown()
	{
		m_geometryShaderStatic  = {};
		m_geometryShaderSkinned = {};
		m_materialSampler.reset();
	}
}
