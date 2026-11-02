#pragma once
#include <Rendering/IRenderPipeline.h>
#include <RayTracing/RTUniforms.h>

namespace bdt
{
	BDT_CLASS_PTR(RTBruteForceBus)
	BDT_CLASS_PTR(RTRayGenerationPass)
	BDT_CLASS_PTR(RTSkinningPass)
	BDT_CLASS_PTR(RTClosestHitPass)
	BDT_CLASS_PTR(RTSecondaryRayPass)
	BDT_CLASS_PTR(RTShadingPass)
	BDT_CLASS_PTR(RTImageResolvePass)
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

		static constexpr uint32 MaxBounces = RTMaxBounces;

		// Render Bus
		RTBruteForceBusUPtr m_bus;

		// Compute Passes
		RTRayGenerationPassUPtr m_rayGenPass;
		RTSkinningPassUPtr m_skinningPass;
		RTClosestHitPassUPtr m_closestHitPass;
		RTSecondaryRayPassUPtr m_secondRayPass;
		RTShadingPassUPtr m_rtShadingPass;
		RTImageResolvePassUPtr m_imageResolvePass;

		// Output Pass
		RTBruteForceOutputPassUPtr m_bruteForceOutputPass;
	};
}