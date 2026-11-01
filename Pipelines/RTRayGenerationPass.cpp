#include "pch.h"
#include "RTRayGenerationPass.h"
#include "RTBruteForceBus.h"

namespace bdt
{
	RTRayGenerationPass::RTRayGenerationPass() = default;

	RTRayGenerationPass::~RTRayGenerationPass()
	{
		Shutdown();
	}

	RTRayGenerationPassUPtr RTRayGenerationPass::Create()
	{
		RTRayGenerationPassUPtr result(new RTRayGenerationPass());
		if (!result->Init()) return nullptr;
		return result;
	}

	bool RTRayGenerationPass::Init()
	{
		m_rayGenerationShader = BDT_RESOURCE.LoadComputeShader
		(
			"RT.BruteForce.RayGeneration",
			{ { GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_brute_force_ray_generate.comp" } }
		);

		return true;
	}

	void RTRayGenerationPass::Execute(IRenderBus& bus)
	{
		auto& rtBus = static_cast<RTBruteForceBus&>(bus);
		const auto shader = m_rayGenerationShader.Get();
		if (!shader || !shader->IsValid()) return;

		const uint32 width = rtBus.GetWidth();
		const uint32 height = rtBus.GetHeight();
		if (width == 0 || height == 0) return;

		auto* rayBuffer = rtBus.GetRayBuffer();
		auto* outputTexture = rtBus.GetOutputTexture();
		if (!rayBuffer || !outputTexture)
			return;

		// 1. Ray SSBO 바인딩
		rayBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Ray));

		// 2. 출력 이미지 바인딩
		glBindImageTexture
		(
			0,
			outputTexture->GetTextureID(),
			0,
			GL_FALSE,
			0,
			GL_WRITE_ONLY,
			GL_RGBA16F
		);

		// 3. Compute Shader 실행
		shader->Bind();
		const uint32 groupX = (width + 7) / 8;
		const uint32 groupY = (height + 7) / 8;
		shader->Dispatch(groupX, groupY, 1);

		// 4. 후속 Compute / Graphics 접근을 위한 메모리 가시성
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

		// 5. 바인딩 해제
		glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		ComputeShader::Unbind();
	}

	void RTRayGenerationPass::Shutdown()
	{
		m_rayGenerationShader = {};
	}
}
