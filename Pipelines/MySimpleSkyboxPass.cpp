#include "pch.h"
#include "MySimpleSkyboxPass.h"
#include "MySimpleBus.h"
#include "MySimpleGBuffer.h"

namespace bdt
{
	MySimpleSkyboxPass::MySimpleSkyboxPass() = default;
	MySimpleSkyboxPass::~MySimpleSkyboxPass() { Shutdown(); }

	MySimpleSkyboxPassUPtr MySimpleSkyboxPass::Create()
	{
		MySimpleSkyboxPassUPtr skyboxPass(new MySimpleSkyboxPass());
		if (!skyboxPass->Init()) return nullptr;
		return skyboxPass;
	}

	bool MySimpleSkyboxPass::Init()
	{
		// 1. Skybox Sampler
		m_skyboxSampler = GLSampler::Create();
		if (!m_skyboxSampler) return false;
		m_skyboxSampler->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		m_skyboxSampler->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		// 2. 스카이박스 셰이더
		m_shader = BDT_RESOURCE.LoadGraphicsShader
		(
			"MySimple.Skybox",
			{
				{ GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_skybox.vert" },
				{ GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_skybox.frag" }
			}
		);

		return true;
	}

	void MySimpleSkyboxPass::Execute(IRenderBus& bus)
	{
		auto* simpleBus = static_cast<MySimpleBus*>(&bus);
		auto* gBuffer = simpleBus->GetGBuffer();
		auto* screen = simpleBus->GetRenderScreen();
		auto shader = m_shader.Get();
		if (!shader || !shader->IsValid()) return;

		// 이 패스가 요청한 이미지에서 IBL을 생성한다.
		auto* position = gBuffer->GetTexture(EGBufferTextureSlot::Position);
		if (!position) return;

		// 1. RenderScreen 바인딩
		screen->Bind();
		glViewport(0, 0, (GLsizei)simpleBus->GetWidth(), (GLsizei)simpleBus->GetHeight());

		// 2. Skybox 렌더링 상태 설정
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_FRAMEBUFFER_SRGB);

		// 3. Texture 및 Sampler 바인딩
		const auto& sky = simpleBus->GetSkybox();
		if (!sky || !sky->IsValid()) return;

		sky->Bind(0);
		m_skyboxSampler->Bind(0);
		position->Bind(1);
		gBuffer->GetSampler()->Bind(1);

		// 4. Skybox 렌더링
		shader->Bind();
		screen->ScreenBlit();

		// 5. 정리
		GraphicsShader::Unbind();
		glBindTextureUnit(0, 0);
		glBindTextureUnit(1, 0);
		glBindSampler(0, 0);
		GLSampler::Unbind(1);

		screen->Unbind();
		glDepthMask(GL_TRUE);
	}

	void MySimpleSkyboxPass::Shutdown()
	{
		m_shader = {};
		m_skyboxSampler.reset();
	}
}
