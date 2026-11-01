#include "pch.h"
#include "MySimpleDeferredShadingPass.h"
#include "MySimpleBus.h"
#include "MySimpleGBuffer.h"

namespace bdt
{
	MySimpleDeferredShadingPass::MySimpleDeferredShadingPass() = default;
	MySimpleDeferredShadingPass::~MySimpleDeferredShadingPass() = default;

	MySimpleDeferredShadingPassUPtr MySimpleDeferredShadingPass::Create()
	{
		MySimpleDeferredShadingPassUPtr result(new MySimpleDeferredShadingPass());
		if (!result->Init()) return nullptr;
		return result;
	}

	bool MySimpleDeferredShadingPass::Init()
	{
		// 샘플러 생성
		m_shadowSampler = GLSampler::Create();
		if (!m_shadowSampler) return false;

		m_shadowSampler->SetFilter(GL_NEAREST, GL_NEAREST);
		m_shadowSampler->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
		const GLfloat borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glSamplerParameterfv(m_shadowSampler->GetSamplerID(), GL_TEXTURE_BORDER_COLOR, borderColor);
		glSamplerParameteri(m_shadowSampler->GetSamplerID(), GL_TEXTURE_COMPARE_MODE, GL_NONE);

		// 2. Irradiance / BRDF LUT Sampler
		m_linearSampler = GLSampler::Create();
		if (!m_linearSampler) return false;
		m_linearSampler->SetFilter(GL_LINEAR, GL_LINEAR);
		m_linearSampler->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		// 3. Prefiltered Environment Sampler
		m_prefilterSampler = GLSampler::Create();
		if (!m_prefilterSampler) return false;
		m_prefilterSampler->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		m_prefilterSampler->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		// 셰이더 로드
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

	void MySimpleDeferredShadingPass::Execute(IRenderBus& bus)
	{
		// 1. RenderBus 및 RenderScene 가져오기
		auto* geometryBus = static_cast<MySimpleBus*>(&bus);
		const RenderScene* scene = geometryBus->GetRenderScene();
		MySimpleGBuffer* gBuffer = geometryBus->GetGBuffer();
		RenderScreen* screen = geometryBus->GetRenderScreen();

		// 2. 비동기 셰이더 로딩 상태 확인
		const auto shader = m_outputShader.Get();
		if (!shader || !shader->IsValid()) return;

		// 3. Shadow Texture Array 획득
		RenderTextureArray2D* shadowTexture = geometryBus->GetShadowTextureArray();
		auto* ibl = scene->GetImageBasedLight();
		const bool iblReady = ibl && ibl->IsValid();

		// 5. 기본 프레임버퍼 바인딩
		screen->Bind();

		// 5. Deferred Shading 상태 설정
		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_FRAMEBUFFER_SRGB);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		// 6. GBuffer Texture 바인딩
		gBuffer->BindTextures();

		// 7. Shadow Texture Array 및 전용 Sampler 바인딩
		const uint32 shadowSlot = static_cast<uint32>(EGBufferTextureSlot::Count);
		glBindTextureUnit(shadowSlot, shadowTexture->GetTextureID());
		m_shadowSampler->Bind(shadowSlot);

		// 6. IBL Textures
		const uint32 irradianceSlot = shadowSlot + 1;
		const uint32 prefilteredSlot = shadowSlot + 2;
		const uint32 brdfSlot = shadowSlot + 3;

		if (iblReady)
		{
			ibl->GetIrradiance()->Bind(irradianceSlot);
			ibl->GetPrefiltered()->Bind(prefilteredSlot);
			ibl->GetBRDFLUT()->Bind(brdfSlot);
		}
		else
		{
			glBindTextureUnit(irradianceSlot, 0);
			glBindTextureUnit(prefilteredSlot, 0);
			glBindTextureUnit(brdfSlot, 0);
		}

		m_linearSampler->Bind(irradianceSlot);
		m_prefilterSampler->Bind(prefilteredSlot);
		m_linearSampler->Bind(brdfSlot);

		// 8. Deferred Shading 실행
		shader->Bind();

		// [TODO] 이후 UBO로 통일 고려 필요
		const GLuint program = shader->GetProgramID();
		const float iblIntensity = iblReady ? ibl->GetIntensity() : 0.0f;
		glProgramUniform1f(program, glGetUniformLocation(program, "uIBLIntensity"), iblIntensity);

		screen->ScreenBlit();

		// 9. 바인딩 해제
		for (uint32 slot = shadowSlot; slot <= brdfSlot; ++slot)
		{
			glBindTextureUnit(slot, 0);
			glBindSampler(slot, 0);
		}
		MySimpleGBuffer::UnbindTextures();
		GraphicsShader::Unbind();
		screen->Unbind();

		glDepthMask(GL_TRUE);
	}

	void MySimpleDeferredShadingPass::Shutdown()
	{
		m_outputShader = {};
		m_shadowSampler.reset();
		m_linearSampler.reset();
		m_prefilterSampler.reset();
	}
}
