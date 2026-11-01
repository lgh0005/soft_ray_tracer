#include "pch.h"
#include "MySimplePipeline.h"
#include "MySimpleBus.h"
#include "MySimpleShadowPass.h"
#include "MySimpleGeometryPass.h"
#include "MySimpleDeferredShadingPass.h"
#include "MySimplePostProcessingPass.h"
#include "MySimpleSkyboxPass.h"

namespace bdt
{
	MySimplePipeline::MySimplePipeline() : Super("MySimplePipeline") { }
	MySimplePipeline::~MySimplePipeline() { Shutdown(); }

	MySimplePipelineUPtr MySimplePipeline::Create()
	{
		MySimplePipelineUPtr result(new MySimplePipeline());
		if (!result->Init()) return nullptr;
		return result;
	}

	bool MySimplePipeline::Init()
	{
		m_bus = MySimpleBus::Create();
		if (!m_bus) return false;

		m_shadowPass = MySimpleShadowPass::Create();
		if (!m_shadowPass) return false;

		m_geometryPass = MySimpleGeometryPass::Create();
		if (!m_geometryPass) return false;

		m_deferredShadingPass = MySimpleDeferredShadingPass::Create();
		if (!m_deferredShadingPass) return false;

		m_skyboxPass = MySimpleSkyboxPass::Create();
		if (!m_skyboxPass) return false;

		m_postProcessingPass = MySimplePostProcessingPass::Create();
		if (!m_postProcessingPass) return false;

		return true;
	}

	bool MySimplePipeline::Resize(uint32 width, uint32 height)
	{
		if (width == 0 || height == 0) return false;
		return m_bus->Resize(width, height);
	}

	void MySimplePipeline::Render(const RenderScene& scene, uint32 width, uint32 height)
	{
		// 창 최소화 시 리소스 재생성 및 렌더링을 생략
		if (width <= 0 || height <= 0) return;

		// 현재 프레임의 렌더 버스 구성
		if (!m_bus->BeginFrame(scene, width, height))
			return;

		// 렌더링 파이프라이닝
		m_shadowPass->Execute(*m_bus);
		m_geometryPass->Execute(*m_bus);
		m_deferredShadingPass->Execute(*m_bus);
		m_skyboxPass->Execute(*m_bus);
		m_postProcessingPass->Execute(*m_bus);
	}

	void MySimplePipeline::Shutdown()
	{
		m_skyboxPass->Shutdown();
		m_deferredShadingPass->Shutdown();
		m_geometryPass->Shutdown();
		m_shadowPass->Shutdown();
		m_bus->Shutdown();
	}
}
