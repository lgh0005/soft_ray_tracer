#include "pch.h"
#include "RTSecondaryRayPass.h"
#include "RTBruteForceBus.h"

namespace bdt
{
	RTSecondaryRayPass::RTSecondaryRayPass() = default;
	RTSecondaryRayPass::~RTSecondaryRayPass()
	{
		Shutdown();
	}

	RTSecondaryRayPassUPtr RTSecondaryRayPass::Create()
	{
		RTSecondaryRayPassUPtr result(new RTSecondaryRayPass());
		if (!result->Init()) return nullptr;
		return result;
	}

	bool RTSecondaryRayPass::Init()
	{
		m_secondaryRayShader = BDT_RESOURCE.LoadComputeShader
		(
			"RT.SecondaryRay",
			{ { GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_brute_force_secondary_ray.comp" } }
		);

		// 2. Prepare Indirect Dispatch
		m_prepareDispatchShader = BDT_RESOURCE.LoadComputeShader
		(
			"RT.PrepareDispatch",
			{ { GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_rt_prepare_dispatch.comp" } }
		);

		return true;
	}

	void RTSecondaryRayPass::Execute(IRenderBus& bus)
	{
		auto& rtBus = static_cast<RTBruteForceBus&>(bus);
		RTRayQueue* queue = rtBus.GetRayQueue();

		auto secondaryShader = m_secondaryRayShader.Get();
		auto prepareShader = m_prepareDispatchShader.Get();
		if (!secondaryShader || !secondaryShader->IsValid() ||
			!prepareShader   || !prepareShader->IsValid()) return;

		// 1. Reset Next Ray Counter
		if (!queue->ResetNextRayCounter())
			return;

		// 2. Current Ray / Hit
		queue->BindRayBuffer();
		queue->BindHitBuffer();
		queue->BindRayCounter();

		// 3. Next Ray / Counter
		queue->BindNextRayBuffer();
		queue->BindNextRayCounter();

		// 4. Geometry
		BDT_INSTANCE.BindStaticVertexBuffer();
		BDT_INSTANCE.BindIndexBuffer();
		BDT_INSTANCE.BindDescriptorBuffer();

		// 5. Instance / GPU Skinning
		BDT_INSTANCE.BindInstanceBuffer();
		BDT_INSTANCE.BindSkinnedOutputBuffer();

		// 6. Compute Shader
		secondaryShader->Bind();
		queue->BindDispatchCommandBuffer();
		glDispatchComputeIndirect(0);

		// Secondary Ray / Counter Write -> Shader Read
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// 7. Prepare Next Dispatch
		prepareShader->Bind();
		queue->BindNextRayCounter();
		queue->BindDispatchCommandBuffer();
		glDispatchCompute(1, 1, 1);

		// 8. Next Bounce Synchronization
		glMemoryBarrier
		(
			GL_SHADER_STORAGE_BARRIER_BIT |
			GL_COMMAND_BARRIER_BIT
		);

		// 9. Cleanup
		queue->UnbindDispatchCommandBuffer();
		ComputeShader::Unbind();
	}

	void RTSecondaryRayPass::Shutdown()
	{
		m_secondaryRayShader = {};
		m_prepareDispatchShader = {};
	}
}
