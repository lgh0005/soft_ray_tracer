#include "pch.h"
#include "MyRTBVHOptimized.h"
#include "RTBVHBus.h"
#include "RTBVHRayGenerationPass.h"
#include "RTBVHSkinningPass.h"
#include "RTBVHClosestHitPass.h"
#include "RTBVHSecondaryRayPass.h"
#include "RTBVHShadingPass.h"
#include "RTBVHImageResolvePass.h"
#include "RTBVHOutputPass.h"

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
		m_bus = RTBVHBus::Create();
		if (!m_bus) return false;

		m_rayGenPass = RTBVHRayGenerationPass::Create();
		if (!m_rayGenPass) return false;

		m_skinningPass = RTBVHSkinningPass::Create();
		if (!m_skinningPass) return false;

		m_closestHitPass = RTBVHClosestHitPass::Create();
		if (!m_closestHitPass) return false;

		m_secondRayPass = RTBVHSecondaryRayPass::Create();
		if (!m_secondRayPass) return false;

		m_rtShadingPass = RTBVHShadingPass::Create();
		if (!m_rtShadingPass) return false;

		m_imageResolvePass = RTBVHImageResolvePass::Create();
		if (!m_imageResolvePass) return false;

		m_outputPass = RTBVHOutputPass::Create();
		if (!m_outputPass) return false;

		return true;
	}

	bool MyRTBVHOptimized::Resize(uint32 width, uint32 height)
	{
		if (width == 0 || height == 0) return false;
		if (!m_bus) return false;
		return m_bus->Resize(GetTraceDimension(width), GetTraceDimension(height));
	}

	void MyRTBVHOptimized::Render(const RenderScene& scene, uint32 width, uint32 height)
	{
		if (width == 0 || height == 0 || !m_bus) return;

		// 1. 프레임 공통 자원 준비
		const uint32 traceWidth = GetTraceDimension(width);
		const uint32 traceHeight = GetTraceDimension(height);
		if (!m_bus->BeginFrame(scene, traceWidth, traceHeight)) return;
		RTRayQueue* queue = m_bus->GetRayQueue();
		if (!queue) return;

		// 2. GPU Skinning
		m_skinningPass->Execute(*m_bus);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// 3. BVH 준비
		// TODO: BVH Build / Refit / Upload 및 Closest Hit용 BVH SSBO 준비

		// 4. Primary Ray Generation
		m_rayGenPass->Execute(*m_bus);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// 5. Bounce Loop
		for (uint32 bounce = 0; bounce <= MaxBounces; ++bounce)
		{
			// BVH Traversal
			m_closestHitPass->Execute(*m_bus);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			// Shading
			m_rtShadingPass->Execute(*m_bus);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			if (bounce == MaxBounces) break;

			// Secondary Ray Generation
			m_secondRayPass->Execute(*m_bus);

			// Ray Queue Swap
			queue->SwapQueues();
		}

		// 6. Image Resolve
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		m_imageResolvePass->Execute(*m_bus);

		// 7. Output
		m_outputPass->Execute(*m_bus);
	}

	void MyRTBVHOptimized::Shutdown()
	{
		m_outputPass.reset();
		m_imageResolvePass.reset();
		m_rtShadingPass.reset();
		m_secondRayPass.reset();
		m_closestHitPass.reset();
		m_skinningPass.reset();
		m_rayGenPass.reset();
		m_bus.reset();
	}
}
