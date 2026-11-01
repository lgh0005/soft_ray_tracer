#include "pch.h"
#include "MySimpleForwardPass.h"
#include "MySimpleBus.h"

namespace bdt
{
	MySimpleForwardPass::~MySimpleForwardPass() { Shutdown(); }
	MySimpleForwardPassUPtr MySimpleForwardPass::Create()
	{
		auto pass = std::make_unique<MySimpleForwardPass>();
		return pass->Init() ? std::move(pass) : nullptr;
	}
	bool MySimpleForwardPass::Init()
	{
		if (m_initialized) return true;
		m_staticShader = BDT_RESOURCE.LoadGraphicsShader("MySimple.Forward.Static",
			{ { GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_forward_static.vert" },
			  { GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_forward.frag" } });
		m_skinnedShader = BDT_RESOURCE.LoadGraphicsShader("MySimple.Forward.Skinned",
			{ { GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_forward_skinned.vert" },
			  { GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_forward.frag" } });
		m_initialized = true;
		return true;
	}
	void MySimpleForwardPass::Execute(IRenderBus& bus)
	{
		//auto* simpleBus = dynamic_cast<MySimpleBus*>(&bus);
		//if (!simpleBus || !m_initialized || !simpleBus->GetRenderScene() || !bus.GetWidth() || !bus.GetHeight()) return;
		//auto staticShader = m_staticShader.Get(), skinnedShader = m_skinnedShader.Get();
		//if (!staticShader || !skinnedShader || !staticShader->IsValid() || !skinnedShader->IsValid()) return;
		//auto* sampler = BDT_RENDER.GetLinearRepeatSampler();
		//auto* aoSampler = BDT_RENDER.GetNearestClampSampler();
		//if (!sampler || !aoSampler || !BDT_RENDER.GetStaticVAO() || !BDT_RENDER.GetSkinnedVAO()) return;
		//MySimplePassState state;
		//if (!simpleBus->PrepareSceneTarget()) return;
		//simpleBus->BindSceneTarget();
		//MySimplePassState::Fullscreen(bus.GetWidth(), bus.GetHeight());
		//glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LESS);
		//glDepthMask(GL_TRUE);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		//glFrontFace(GL_CCW);
		//const GLfloat black[4]{ 0.0f, 0.0f, 0.0f, 1.0f }, depth = 1.0f;
		//glClearBufferfv(GL_COLOR, 0, black);
		//glClearBufferfv(GL_DEPTH, 0, &depth);
		//for (uint32 slot = 0; slot < static_cast<uint32>(EMaterialTextureSlot::Count); ++slot)
		//	glBindSampler(slot, sampler->GetSamplerID());
		//// 머티리얼이 사용하는 0..7과 충돌하지 않는 슬롯.
		//auto* ao = simpleBus->GetSSAOTexture();
		//if (ao) ao->Bind(8);
		//glBindSampler(8, aoSampler->GetSamplerID());
		//for (const auto& shader : { staticShader, skinnedShader })
		//{
		//	glProgramUniform1i(shader->GetProgramID(), glGetUniformLocation(shader->GetProgramID(), "uHasSSAO"), ao != nullptr);
		//	glProgramUniform1ui(shader->GetProgramID(), glGetUniformLocation(shader->GetProgramID(), "uLightCount"),
		//		static_cast<uint32>(simpleBus->GetRenderScene()->GetLights().size()));
		//}
		//// 불투명 씬 전체를 그리는 Forward 대체 경로. 투명 정렬은 별도 계약이다.
		//simpleBus->ExecuteRenderQueues(staticShader, skinnedShader);
		//simpleBus->SetSceneColorReady();
	}
	void MySimpleForwardPass::Shutdown()
	{
		m_staticShader = {};
		m_skinnedShader = {};
		m_initialized = false;
	}
}
