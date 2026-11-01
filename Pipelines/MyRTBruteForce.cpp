#include "pch.h"
#include "MyRTBruteForce.h"

#include "RTBruteForceBus.h"
#include "RTRayGenerationPass.h"
#include "RTSkinningPass.h"
#include "RTClosestHitPass.h"
#include "RTSecondaryRayPass.h"
#include "RTShadingPass.h"
#include "RTBruteForceOutputPass.h"

namespace bdt
{
    namespace
    {
        // 가로·세로를 원본의 1/4로 줄여 초기 광선 수를 약 1/16로 제한한다.
        constexpr uint32 GetTraceDimension(uint32 dimension)
        {
            return dimension / 4 + (dimension % 4 != 0 ? 1u : 0u);
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

		//m_secondRayPass = RTSecondaryRayPass::Create();
		//if (!m_secondRayPass) return false;

		// 3. Render Pass 생성
		m_rtShadingPass = RTShadingPass::Create();
		if (!m_rtShadingPass) return false;

		m_bruteForceOutputPass = RTBruteForceOutputPass::Create();
		if (!m_bruteForceOutputPass) return false;

		return true;
	}

	bool MyRTBruteForce::Resize(uint32 width, uint32 height)
	{
		if (width == 0 || height == 0)
			return false;

		if (!m_bus)
			return false;

		return m_bus->Resize(GetTraceDimension(width), GetTraceDimension(height));
	}

	void MyRTBruteForce::Render(const RenderScene& scene, uint32 width, uint32 height)
	{
		// 1. 최소화 상태 및 초기화 여부 확인
		if (width == 0 || height == 0 || !m_bus)
			return;

		// 2. 프레임 공통 자원 준비
		if (!m_bus->BeginFrame(scene, GetTraceDimension(width), GetTraceDimension(height)))
			return;

		// 3. Primary Ray 생성
		m_rayGenPass->Execute(*m_bus);

		// 4. GPU 스키닝
		m_skinningPass->Execute(*m_bus);

		// 5. Primary Ray의 최근접 교차점 탐색
		m_closestHitPass->Execute(*m_bus);

		// 6. 교차 결과에 대한 음영 계산
		m_rtShadingPass->Execute(*m_bus);

		// Secondary Ray 처리 (미활성)
		// TODO:
		// Reflection / Refraction 반복 처리 구조 확정 후 활성화
		// m_secondRayPass->Execute(*m_bus);

		// 7. 최종 이미지 출력 (ImageResolve)
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
		// m_bus->Shutdown();
	}
}
