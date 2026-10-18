#include "pch.h"
#include "MySimpleBus.h"
#include <Core/GLBuffer.h>
#include <Rendering/InstanceBuffer.h>
#include <Rendering/RenderScene.h>
#include "MyGBuffer.h"

namespace bdt
{
	MySimpleBus::MySimpleBus() = default;
	MySimpleBus::~MySimpleBus() = default;

	MySimpleBusUPtr MySimpleBus::Create()
	{
		MySimpleBusUPtr bus(new MySimpleBus());
		if (!bus->Init()) return nullptr;
		return bus;
	}

	bool MySimpleBus::Init()
	{
		// 카메라 및 조명 UBO 생성
		m_cameraBuffer = GLBuffer::Create(sizeof(CameraData), nullptr, GL_DYNAMIC_STORAGE_BIT);
		m_lightBuffer = InstanceBuffer::Create();
		if (!m_cameraBuffer || !m_lightBuffer) return false;
		return true;
	}

	bool MySimpleBus::Resize(uint32 width, uint32 height)
	{
		if (width == 0 || height == 0)
			return false;

		// 최초 생성
		if (m_gBuffer &&
			m_gBuffer->GetWidth()  == width &&
			m_gBuffer->GetHeight() == height)
		{
			return true;
		}

		// 기존 리소스의 크기 변경
		if (!m_gBuffer)
		{
			m_gBuffer = MyGBuffer::Create(width, height);
			if (!m_gBuffer) return false;
		}
		else if (!m_gBuffer->Resize(width, height))
		{
			return false;
		}

		SetSize(width, height);
		return true;
	}

	bool MySimpleBus::BeginFrame(const RenderScene& scene, uint32 width, uint32 height)
	{
		if (!Resize(width, height))
			return false;

		// 버스에 RenderScene 주소 세팅
		SetRenderScene(&scene);

		// Camera UBO 업로드
		CameraData camera = scene.GetCamera();
		camera.viewportSize = vec2(width, height);
		m_cameraBuffer->SetData(0, sizeof(CameraData), &camera);
		m_cameraBuffer->BindBase(GL_UNIFORM_BUFFER, (uint32)EUniformBinding::Camera);

		// 조명 SSBO 업로드
		const List<LightData>& lights = scene.GetLights();
		const uint32 lightCount = static_cast<uint32>(lights.size());
		m_lightBuffer->Update(lights.data(), lightCount * static_cast<uint32>(sizeof(LightData)));
		if (m_lightBuffer->GetSize() != lightCount * static_cast<uint32>(sizeof(LightData)))
			return false;

		m_lightCount = lightCount;
		m_lightBuffer->Bind((uint32)EStorageBinding::Light);

		return true;
	}

	void MySimpleBus::Shutdown()
	{
		m_gBuffer.reset();
		m_lightBuffer.reset();
		m_cameraBuffer.reset();

		m_lightCount = 0;
		SetRenderScene(nullptr);
		SetSize(0, 0);
	}

	MyGBuffer* MySimpleBus::GetGBuffer() const
	{
		return m_gBuffer.get();
	}

	uint32 MySimpleBus::GetLightCount() const
	{
		return m_lightCount;
	}
}
