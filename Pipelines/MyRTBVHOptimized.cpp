#include "pch.h"
#include "MyRTBVHOptimized.h"
#include "RTBVHBus.h"
#include "RTBVHRayGenerationPass.h"
#include "RTBVHSkinningPass.h"
#include "RTBVHRefitPass.h"
#include "RTBVHClosestHitPass.h"
#include "RTBVHSecondaryRayPass.h"
#include "RTBVHShadingPass.h"
#include "RTBVHImageResolvePass.h"
#include "RTBVHSurfacePreparePass.h"
#include "RTBVHLightingPass.h"
#include "RTBVHOutputPass.h"
#include "RTBVHDenoisingPass.h"
#include "RTBVHAOPass.h"

namespace bdt
{

	MyRTBVHOptimized::MyRTBVHOptimized() : IRenderPipeline("MyRTBVHOptimized") { }

	MyRTBVHOptimized::~MyRTBVHOptimized()
	{
		Shutdown();
	}

	MyRTBVHOptimizedUPtr MyRTBVHOptimized::Create()
	{
		MyRTBVHOptimizedUPtr result(new MyRTBVHOptimized());
		if (!result->Init()) return nullptr;
		return result;
	}

	bool MyRTBVHOptimized::Init()
	{
		if (m_bus) return true;
		m_bus = RTBVHBus::Create();
		if (!m_bus) return false;

		m_rayGenPass = RTBVHRayGenerationPass::Create();
		if (!m_rayGenPass) return false;

		m_skinningPass = RTBVHSkinningPass::Create();
		if (!m_skinningPass) return false;

		m_refitPass = RTBVHRefitPass::Create();
		if (!m_refitPass) return false;

		m_closestHitPass = RTBVHClosestHitPass::Create();
		if (!m_closestHitPass) return false;

		m_secondRayPass = RTBVHSecondaryRayPass::Create();
		if (!m_secondRayPass) return false;

		m_rtShadingPass = RTBVHShadingPass::Create();
		if (!m_rtShadingPass) return false;

		m_imageResolvePass = RTBVHImageResolvePass::Create();
		if (!m_imageResolvePass) return false;

		m_surfacePreparePass = RTBVHSurfacePreparePass::Create();
		if (!m_surfacePreparePass) return false;

		m_lightingPass = RTBVHLightingPass::Create();
		if (!m_lightingPass) return false;

		m_denoisingPass = RTBVHDenoisingPass::Create();
		if (!m_denoisingPass) return false;

		m_aoPass = RTBVHAOPass::Create();
		if (!m_aoPass) return false;

		m_outputPass = RTBVHOutputPass::Create();
		if (!m_outputPass) return false;

		return true;
	}

    RTBVHBus* MyRTBVHOptimized::GetBus() const
    {
        return m_bus.get();
    }


	bool MyRTBVHOptimized::Resize(uint32 width, uint32 height)
	{
		if (width == 0 || height == 0) return false;
		if (!m_bus) return false;
		return m_bus->Resize(width, height);
	}

	void MyRTBVHOptimized::Render(const RenderScene& scene, uint32 width, uint32 height)
	{
		// 0. 최소화 상태 및 초기화 여부 확인
		if (width == 0 || height == 0 || !m_bus)
			return;

		// 1. 프레임 공통 자원 준비
		if (!m_bus->BeginFrame(scene, width, height)) return;
		const auto& settings = m_bus->GetRenderSettings();
		RTRayQueue* queue = m_bus->GetRayQueue();
		if (!queue) return;

		// 2. GPU Skinning
		if (!m_skinningPass->Execute(*m_bus)) return;

		// 3. GPU BLAS / TLAS Refit
		if (!m_refitPass->Execute(*m_bus)) return;

		// 4. Primary Ray Generation
		if (!m_rayGenPass->Execute(*m_bus)) return;

		// 5. Bounce Loop
		for (uint32 bounce = 0; bounce <= settings.maxBounces; ++bounce)
		{
			// BVH Traversal
			if (!m_closestHitPass->Execute(*m_bus)) return;

			// Surface Prepare
			if (!m_surfacePreparePass->Execute(*m_bus)) return;

			// AO is a diagnostic input; MC environment lighting already traces visibility.
			if (settings.visualize && !m_aoPass->Execute(*m_bus)) return;

			// Direct Lighting
			if (!settings.visualize && !m_lightingPass->Execute(*m_bus)) return;

			// Shading
			if (!m_rtShadingPass->Execute(*m_bus)) return;
			if (settings.visualize || bounce == settings.maxBounces) break;

			// Secondary Ray Generation
			if (!m_secondRayPass->Execute(*m_bus)) return;

			// Ray Queue Swap
			queue->SwapQueues();
		}

		// 6. Image Resolve
		if (!m_imageResolvePass->Execute(*m_bus)) return;

		if (!settings.visualize && !m_denoisingPass->Execute(*m_bus)) return;

		// 7. Output
		if (!m_outputPass->Execute(*m_bus)) return;
	}

	void MyRTBVHOptimized::Shutdown()
	{
		m_outputPass.reset();
		m_denoisingPass.reset();
		m_aoPass.reset();
		m_lightingPass.reset();
		m_surfacePreparePass.reset();
		m_imageResolvePass.reset();
		m_rtShadingPass.reset();
		m_secondRayPass.reset();
		m_closestHitPass.reset();
		m_refitPass.reset();
		m_skinningPass.reset();
		m_rayGenPass.reset();
		m_bus.reset();
	}
}
