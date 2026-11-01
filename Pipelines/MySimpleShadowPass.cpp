#include "pch.h"
#include "MySimpleShadowPass.h"
#include "MySimpleBus.h"

namespace bdt
{
	MySimpleShadowPass::MySimpleShadowPass(uint32 resolution) : m_resolution(resolution) { }
	MySimpleShadowPass::~MySimpleShadowPass() = default;

	MySimpleShadowPassUPtr MySimpleShadowPass::Create(uint32 resolution)
	{
		MySimpleShadowPassUPtr result(new MySimpleShadowPass(resolution));
		if (!result->Init()) return nullptr;
		return result;
	}

	bool MySimpleShadowPass::Init()
	{
		// static/skinned 깊이 셰이더 비동기 로드 요청
		m_shadowShaderStatic = BDT_RESOURCE.LoadGraphicsShader
		(
			"MyLocalShadow.Static",
			{
				{ GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_shadow_static.vert" },
				{ GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_shadow.frag" }
			}
		);

		m_shadowShaderSkinned = BDT_RESOURCE.LoadGraphicsShader
		(
			"MyLocalShadow.Skinned",
			{
				{ GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_shadow_skinned.vert" },
				{ GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_shadow.frag" }
			}
		);

		return true;
	}

    void MySimpleShadowPass::Execute(IRenderBus& bus)
    {
        auto* shadowBus = static_cast<MySimpleBus*>(&bus);
        const RenderScene* scene = shadowBus->GetRenderScene();
        const auto& shadows = scene->GetShadows();
        if (shadows.empty()) return;

        // 1. Shadow Target Array 준비
		uint32 layerCount = 0;
		for (const ShadowData& shadow : shadows)
		{
			if (shadow.shadowMapIndex == UINT32_MAX) return;
			layerCount = std::max(layerCount, shadow.shadowMapIndex + 1);
		}

		if (!shadowBus->PrepareShadowTarget(m_resolution, layerCount)) 
			return;

        Framebuffer2D* framebuffer = shadowBus->GetShadowFramebuffer();
        if (!framebuffer) return;

        // 2. Shadow Rendering 상태 설정
        framebuffer->Bind();
        glViewport(0, 0, static_cast<GLsizei>(m_resolution), static_cast<GLsizei>(m_resolution));

		// 3. Shadow Shader 준비
		auto staticShader = m_shadowShaderStatic.Get();
		auto skinnedShader = m_shadowShaderSkinned.Get();
		if (!staticShader || !staticShader->IsValid()) return;
		if (!skinnedShader || !skinnedShader->IsValid()) return;

        // 7. 광원 Shadow Map 인스턴싱
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.1f, 2.0f);
		glClearDepth(1.0);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);
		shadowBus->ExecuteShadowRenderQueues(staticShader, skinnedShader);

		glDisable(GL_POLYGON_OFFSET_FILL);
    }

	void MySimpleShadowPass::Shutdown()
	{
		m_shadowShaderStatic = {};
		m_shadowShaderSkinned = {};
	}
}
