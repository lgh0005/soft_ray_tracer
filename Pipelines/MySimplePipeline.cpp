#include "pch.h"
#include "MySimplePipeline.h"

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

		m_geometryPass = MySimpleGeometryPass::Create();
		if (!m_geometryPass) return false;

		m_outputPass = MySimpleOutputPass::Create();
		if (!m_outputPass) return false;

		return true;
	}

	bool MySimplePipeline::Resize(uint32 width, uint32 height)
	{
		if (width == 0 || height == 0) return false;
		return m_bus->Resize(width, height);
	}

	void MySimplePipeline::Render(const RenderScene& scene, uint32 width, uint32 height)
	{
		// 창 최소화 시 리소스 재생성 및 렌더링을 생략한다.
		if (width <= 0 || height <= 0) return;

		// 현재 프레임의 렌더 버스 구성
		if (!m_bus->BeginFrame(scene, width, height))
			return;

		// 렌더 파이프라이닝
		m_geometryPass->Execute(*m_bus);
		m_outputPass->Execute(*m_bus);
	}

	void MySimplePipeline::Shutdown()
	{
		if (m_outputPass) m_outputPass->Shutdown();
		if (m_geometryPass) m_geometryPass->Shutdown();
		if (m_bus) m_bus->Shutdown();

		m_outputPass.reset();
		m_geometryPass.reset();
		m_bus.reset();
	}
}
