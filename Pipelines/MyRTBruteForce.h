#pragma once
#include <Rendering/IRenderPipeline.h>

namespace bdt
{
	BDT_CLASS_PTR(RTBruteForceBus)
	BDT_CLASS_PTR(RTRayGenerationPass)
	BDT_CLASS_PTR(RTSkinningPass)
	BDT_CLASS_PTR(RTClosestHitPass)
	BDT_CLASS_PTR(RTSecondaryRayPass)
	BDT_CLASS_PTR(RTShadingPass)
	BDT_CLASS_PTR(RTBruteForceOutputPass)
	BDT_CLASS_PTR(MyRTBruteForce)

	class MyRTBruteForce : public IRenderPipeline
	{
		BDT_DISABLE_COPY(MyRTBruteForce)
		BDT_DISABLE_MOVE(MyRTBruteForce)
		using Super = IRenderPipeline;

	public:
		virtual ~MyRTBruteForce() override;
		static MyRTBruteForceUPtr Create();
		virtual bool Resize(uint32 width, uint32 height) override;
		virtual void Render(const RenderScene& scene, uint32 width, uint32 height) override;
		virtual void Shutdown() override;

	private:
		MyRTBruteForce();
		virtual bool Init() override;

		// Render Bus
		RTBruteForceBusUPtr m_bus;

		// Compute Passes
		RTRayGenerationPassUPtr m_rayGenPass;
		RTSkinningPassUPtr m_skinningPass;
		RTClosestHitPassUPtr m_closestHitPass;
		RTSecondaryRayPassUPtr m_secondRayPass;

		// Render Passes
		RTShadingPassUPtr m_rtShadingPass;
		RTBruteForceOutputPassUPtr m_bruteForceOutputPass;
	};
}