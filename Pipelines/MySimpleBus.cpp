#include "pch.h"
#include "MySimpleBus.h"
#include "MySimpleGBuffer.h"

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
		// UBO 및 SSBO 생성
		m_cameraBuffer = GLBuffer::Create(sizeof(CameraData), nullptr, GL_DYNAMIC_STORAGE_BIT);
		m_skyboxBuffer = GLBuffer::Create(sizeof(SkyboxData), nullptr, GL_DYNAMIC_STORAGE_BIT);
		m_lightBuffer = GLStorageBuffer::Create();
		m_shadowBuffer = GLStorageBuffer::Create();
		if (!m_cameraBuffer || !m_skyboxBuffer || !m_lightBuffer || !m_shadowBuffer)
			return false;

		// 공유 RenderQueue 생성
		m_staticQueue = StaticRenderQueue::Create();
		m_skinnedQueue = SkinnedRenderQueue::Create();
		if (!m_staticQueue || !m_skinnedQueue) return false;

		// 스크린 렌더 공간 생성
		m_screen = RenderScreen::Create(BDT_WINDOW.GetWindowWidth(), BDT_WINDOW.GetWindowHeight());
		if (!m_screen) return false;

		return true;
	}

	bool MySimpleBus::Resize(uint32 width, uint32 height)
	{
		if (width == 0 || height == 0)
			return false;

		// 최초 생성
		if (m_gBuffer &&
			m_gBuffer->GetWidth() == width &&
			m_gBuffer->GetHeight() == height &&
			m_screen->GetWidth() == width &&
			m_screen->GetHeight() == height) return true;

		// 스크린 리사이징
		if (!m_screen->Resize(width, height))
			return false;

		// 기존 리소스의 크기 변경
		if (!m_gBuffer)
		{
			m_gBuffer = MySimpleGBuffer::Create(width, height);
			if (!m_gBuffer) return false;
		}
		else if (!m_gBuffer->Resize(width, height))
			return false;

		// LDR Render Target 생성 및 크기 변경
		if (!m_ldrTexture ||
			m_ldrTexture->GetWidth() != width ||
			m_ldrTexture->GetHeight() != height)
		{
			auto texture = RenderTexture2D::Create(width, height, GL_RGBA8);
			if (!texture) return false;

			auto framebuffer = Framebuffer2D::Create(width, height);
			if (!framebuffer) return false;

			if (!framebuffer->AttachColor(0, texture->GetTextureID()))
				return false;

			framebuffer->SetDrawBuffers(1);
			m_ldrTexture = std::move(texture);
			m_ldrFramebuffer = std::move(framebuffer);
		}

		SetSize(width, height);
		return true;
	}

	bool MySimpleBus::BeginFrame(const RenderScene& scene, uint32 width, uint32 height)
	{
		// 1. 렌더 큐 초기화
		m_staticQueue->Clear();
		m_skinnedQueue->Clear();

		// 2. 광원 및 그림자 데이터 획득
		const List<LightData>& lights = scene.GetLights();
		const List<ShadowData>& shadows = scene.GetShadows();

		// 3. 렌더 타깃 크기 변경
		if (!Resize(width, height))
			return false;

		// 4. RenderScene 설정
		SetRenderScene(&scene);

		// 5. Camera UBO 업로드
		CameraData camera = scene.GetCamera();
		camera.viewportSize = vec2(width, height);
		m_cameraBuffer->SetData(0, sizeof(CameraData), &camera);
		m_cameraBuffer->BindBase(GL_UNIFORM_BUFFER, (uint32)EUniformBinding::Camera);

		// 6. Skybox UBO 업로드
		SkyboxData skybox{};
		skybox.invProjection = glm::inverse(camera.projection);
		skybox.invViewRotation = mat4(glm::transpose(glm::mat3(camera.view)));

		auto* ibl = scene.GetImageBasedLight();
		SetSkybox(nullptr);
		skybox.intensity = 0.0f;
		if (ibl)
		{
			skybox.intensity = ibl->GetIntensity();
			if (ibl->Generate()) SetSkybox(ibl->GetEnvironment());
		}

		m_skyboxBuffer->SetData(0, sizeof(SkyboxData), &skybox);
		m_skyboxBuffer->BindBase(GL_UNIFORM_BUFFER, static_cast<uint32>(EUniformBinding::ScreenSpace));

		// 7. 조명 SSBO 업로드
		if (!lights.empty())
		{
			if (!m_lightBuffer->Update(lights.data(), lights.size() * sizeof(LightData)))
				return false;
		}
		m_lightBuffer->Bind((uint32)EStorageBinding::Light);

		// 8. 그림자 SSBO 업로드
		if (!shadows.empty())
		{
			if (!m_shadowBuffer->Update(shadows.data(), shadows.size() * sizeof(ShadowData)))
				return false;
		}
		m_shadowBuffer->Bind((uint32)EStorageBinding::Shadow);

		// 9. RenderQueue에 객체 등록
		for (const RenderObject* object : scene.GetObjects())
		{
			if (object)
				SubmitObjectToRenderQueue(*object);
		}

		for (const auto* group : scene.GetObjectGroups())
		{
			if (!group) continue;
			for (const RenderObject& object : *group)
				SubmitObjectToRenderQueue(object);
		}

		// 10. Geometry Instance SSBO 준비
		if (!m_staticQueue->Prepare())
			return false;

		if (!m_skinnedQueue->Prepare())
			return false;

		// 11. Shadow Instance SSBO 준비
		if (!m_staticQueue->PrepareShadow(shadows))
			return false;

		if (!m_skinnedQueue->PrepareShadow(shadows))
			return false;

		return true;
	}

	void MySimpleBus::Shutdown()
	{
		m_skyboxTexture.reset();
		m_staticQueue.reset();
		m_skinnedQueue.reset();

		m_shadowTextureArray.reset();
		m_shadowFramebuffer.reset();

		m_gBuffer.reset();
		m_shadowBuffer.reset();
		m_lightBuffer.reset();
		m_cameraBuffer.reset();
		m_skyboxBuffer.reset();
		m_screen.reset();
		m_ldrFramebuffer.reset();
		m_ldrTexture.reset();

		SetRenderScene(nullptr);
		SetSize(0, 0);
	}

	MySimpleGBuffer* MySimpleBus::GetGBuffer() const
	{
		return m_gBuffer.get();
	}

	void MySimpleBus::SubmitObjectToRenderQueue(const RenderObject& object)
	{
		// static/skinned 공유 큐에 객체 등록
		const auto& mesh = object.GetMesh();
		const auto material = object.GetMaterial().Get();
		if (object.IsSkinned())
		{
			const auto bones = object.GetBoneMatrices();
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

	void MySimpleBus::ExecuteRenderQueues
	(
		const GraphicsShaderPtr& staticShader,
		const GraphicsShaderPtr& skinnedShader
	)
	{
		if (staticShader && staticShader->IsValid())
			m_staticQueue->Execute(staticShader.get());

		if (skinnedShader && skinnedShader->IsValid())
			m_skinnedQueue->Execute(skinnedShader.get());
	}

	void MySimpleBus::ExecuteShadowRenderQueues
	(
		const GraphicsShaderPtr& staticShader,
		const GraphicsShaderPtr& skinnedShader
	)
	{
		if (staticShader && staticShader->IsValid())
			m_staticQueue->ExecuteShadow(staticShader.get());

		if (skinnedShader && skinnedShader->IsValid())
			m_skinnedQueue->ExecuteShadow(skinnedShader.get());
	}

	bool MySimpleBus::PrepareShadowTarget(uint32 resolution, uint32 layers)
	{
		if (resolution == 0 || layers == 0)
			return false;

		// 1. Shadow Texture Array 생성 및 크기 변경
		if (!m_shadowTextureArray)
		{
			m_shadowTextureArray = RenderTextureArray2D::Create
			(
				resolution,
				resolution,
				layers,
				GL_DEPTH_COMPONENT32F
			);

			if (!m_shadowTextureArray)
				return false;
		}
		else
		{
			const bool resolutionChanged =
				m_shadowTextureArray->GetWidth() != resolution ||
				m_shadowTextureArray->GetHeight() != resolution;

			const bool insufficientLayers =
				m_shadowTextureArray->GetLayers() < layers;

			if (resolutionChanged || insufficientLayers)
			{
				const uint32 targetLayers = glm::max(layers, m_shadowTextureArray->GetLayers());
				if (!m_shadowTextureArray->Resize(resolution, resolution, targetLayers))
					return false;
			}
		}

		// 2. Shadow Texture Sampling 설정
		const GLuint textureID = m_shadowTextureArray->GetTextureID();
		glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTextureParameteri(textureID, GL_TEXTURE_COMPARE_MODE, GL_NONE);

		const GLfloat borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTextureParameterfv(textureID, GL_TEXTURE_BORDER_COLOR, borderColor);

		// 3. Shadow Framebuffer 생성
		if (!m_shadowFramebuffer)
		{
			m_shadowFramebuffer = Framebuffer2D::Create(resolution, resolution);
			if (!m_shadowFramebuffer)
				return false;
		}

		// 4. Texture Array 전체를 Depth Attachment로 연결
		m_shadowFramebuffer->Bind();

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureID, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		// 5. Framebuffer 완전성 검사
		return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}

	Framebuffer2D* MySimpleBus::GetLDRFramebuffer() const
	{
		return m_ldrFramebuffer.get();
	}

	RenderTexture2D* MySimpleBus::GetLDRTexture() const
	{
		return m_ldrTexture.get();
	}

	Framebuffer2D* MySimpleBus::GetShadowFramebuffer() const
	{
		return m_shadowFramebuffer.get();
	}

	RenderTextureArray2D* MySimpleBus::GetShadowTextureArray() const
	{
		return m_shadowTextureArray.get();
	}

	RenderScreen* MySimpleBus::GetRenderScreen() const
	{
		return m_screen.get();
	}
}
