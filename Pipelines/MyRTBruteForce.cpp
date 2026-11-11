#include "pch.h"
#include "MyRTBruteForce.h"

#include "RTBruteForceBus.h"
#include "RTRayGenerationPass.h"
#include "RTSkinningPass.h"
#include "RTClosestHitPass.h"
#include "RTSecondaryRayPass.h"
#include "RTShadingPass.h"
#include "RTImageResolvePass.h"
#include "RTBruteForceOutputPass.h"

namespace bdt
{
    namespace
    {
        // 1: 원본 해상도, 2: 가로·세로 절반, 4: 가로·세로 1/4
        constexpr uint32 TraceResolutionDivisor = 1;
        constexpr uint32 GetTraceDimension(uint32 dimension)
        {
            return dimension / TraceResolutionDivisor + (dimension % TraceResolutionDivisor != 0 ? 1u : 0u);
        }
    }

	MyRTBruteForce::MyRTBruteForce() : IRenderPipeline("MyRTBruteForce") { }

	MyRTBruteForce::~MyRTBruteForce()
	{
		Shutdown();
	}

	MyRTBruteForceUPtr MyRTBruteForce::Create()
	{
		MyRTBruteForceUPtr result(new MyRTBruteForce());
		if (!result->Init()) return nullptr;
		return result;
	}

	bool MyRTBruteForce::Init()
	{
		// 1. Render Bus 생성
		m_bus = RTBruteForceBus::Create();
		if (!m_bus) return false;

		// 2. Compute Pass 생성
		m_rayGenPass = RTRayGenerationPass::Create();
		if (!m_rayGenPass) return false;

		m_skinningPass = RTSkinningPass::Create();
		if (!m_skinningPass) return false;

		m_closestHitPass = RTClosestHitPass::Create();
		if (!m_closestHitPass) return false;

		m_secondRayPass = RTSecondaryRayPass::Create();
		if (!m_secondRayPass) return false;

		m_rtShadingPass = RTShadingPass::Create();
		if (!m_rtShadingPass) return false;

		// 3. Image Resolve
		m_imageResolvePass = RTImageResolvePass::Create();
		if (!m_imageResolvePass) return false;

		// 4. Render Pass 생성
		m_bruteForceOutputPass = RTBruteForceOutputPass::Create();
		if (!m_bruteForceOutputPass) return false;

		return true;
	}

	bool MyRTBruteForce::Resize(uint32 width, uint32 height)
	{
		if (width == 0 || height == 0) return false;
		if (!m_bus) return false;
		return m_bus->Resize(GetTraceDimension(width), GetTraceDimension(height));
	}

	void MyRTBruteForce::Render(const RenderScene& scene, uint32 width, uint32 height)
	{
		// 1. 최소화 상태 및 초기화 여부 확인
		if (width == 0 || height == 0 || !m_bus)
			return;

		// 2. 프레임 공통 자원 준비
		const uint32 traceWidth = GetTraceDimension(width);
		const uint32 traceHeight = GetTraceDimension(height);
		if (!m_bus->BeginFrame(scene, traceWidth, traceHeight)) return;
		RTRayQueue* queue = m_bus->GetRayQueue();
		if (!queue) return;

		// 2. GPU Skinning
		m_skinningPass->Execute(*m_bus);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// 3. Primary Ray Generation
		m_rayGenPass->Execute(*m_bus);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// 4. Bounce Loop
		for (uint32 bounce = 0; bounce <= MaxBounces; ++bounce)
		{
			// Current Ray Queue -> Closest Hit
			m_closestHitPass->Execute(*m_bus);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			// Hit -> Radiance Accumulation
			m_rtShadingPass->Execute(*m_bus);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			// 마지막 Bounce에서는 후속 광선을 생성하지 않음
			if (bounce == MaxBounces) break;

			// Secondary Ray Generation
			// + Atomic Queue Compaction
			// + Prepare Indirect Dispatch
			m_secondRayPass->Execute(*m_bus);

			// Current / Next Ray Queue 교체
			queue->SwapQueues();
		}

		// 5. Accumulation -> Output Texture
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		m_imageResolvePass->Execute(*m_bus);

		// 6. Output Texture -> Screen
		m_bruteForceOutputPass->Execute(*m_bus);
	}

	void MyRTBruteForce::Shutdown()
	{
		if (m_bruteForceOutputPass) m_bruteForceOutputPass->Shutdown();
		if (m_rtShadingPass) m_rtShadingPass->Shutdown();
		if (m_secondRayPass) m_secondRayPass->Shutdown();
		if (m_closestHitPass) m_closestHitPass->Shutdown();
		if (m_skinningPass) m_skinningPass->Shutdown();
		if (m_rayGenPass) m_rayGenPass->Shutdown();
		m_bus->Shutdown();
	}
}
