#include "pch.h"
#include "MySimpleOutputPass.h"
#include "MySimpleBus.h"
#include "MyGBuffer.h"

#include <Resources/Resource.h>
#include <Resources/GraphicsShader.h>

#include <Core/GLSampler.h>
#include <Core/GLVertexArray.h>
#include <Core/GLVertices.h>
#include <Core/GLMesh.h>
#include <Core/GLBuffer.h>

namespace bdt
{
	MySimpleOutputPass::MySimpleOutputPass() = default;
	MySimpleOutputPass::~MySimpleOutputPass() = default;

	MySimpleOutputPassUPtr MySimpleOutputPass::Create()
	{
		MySimpleOutputPassUPtr result(new MySimpleOutputPass());
		if (!result->Init()) return nullptr;
		return result;
	}

	bool MySimpleOutputPass::Init()
	{
		m_outputShader = BDT_RESOURCE.LoadGraphicsShader
		(
			"MyLocal.Output",
			{
				{ GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_output_pass.vert" },
				{ GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_output_pass.frag" }
			}
		);

		return true;
	}

	void MySimpleOutputPass::Execute(IRenderBus& bus)
	{
		// 1. RenderBus 및 RenderScene 가져오기
		auto* geometryBus = static_cast<MySimpleBus*>(&bus);
		if (!geometryBus) return;

		MyGBuffer* gBuffer = geometryBus->GetGBuffer();
		if (!gBuffer) return;

		// 2. 비동기 셰이더 로딩 상태 확인
		const auto shader = m_outputShader.Get();
		if (!shader) return;

		// 3. 공유 Sampler 확인
		GLSampler* sampler = BDT_RENDER.GetNearestClampSampler();
		if (!sampler) return;

		// 4. RenderScreen 크기 갱신
		const uint32 width = geometryBus->GetWidth();
		const uint32 height = geometryBus->GetHeight();
		if (width == 0 || height == 0) return;
		if (!m_renderScreen.Resize(width, height)) return;

		// 5. 기본 프레임버퍼 바인딩
		m_renderScreen.Bind();

		// 6. 렌더링 상태 설정
		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_FRAMEBUFFER_SRGB);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		// 7. GBuffer 텍스처 및 Sampler 바인딩
		gBuffer->BindTextures();

		for (uint32 slot = 0; slot < static_cast<uint32>(EGBufferTextureSlot::Count); ++slot)
			glBindSampler(slot, sampler->GetSamplerID());

		// 8. Deferred Lighting Shader
		shader->Bind();

		// 9. Fullscreen Draw
		m_renderScreen.Draw();

		// 10. 바인딩 해제
		m_renderScreen.Unbind();
		glUseProgram(0);
		MyGBuffer::UnbindTextures();
		for (uint32 slot = 0; slot < static_cast<uint32>(EGBufferTextureSlot::Count); ++slot)
			glBindSampler(slot, 0);

		glDepthMask(GL_TRUE);
	}

	void MySimpleOutputPass::Shutdown()
	{
		m_outputShader = {};
	}
}
