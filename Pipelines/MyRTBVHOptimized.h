#pragma once
#include <Rendering/IRenderPipeline.h>

namespace bdt
{
	BDT_CLASS_PTR(RTBVHBus)
	BDT_CLASS_PTR(RTBVHRayGenerationPass)
	BDT_CLASS_PTR(RTBVHSkinningPass)
	BDT_CLASS_PTR(RTBVHRefitPass)
	BDT_CLASS_PTR(RTBVHClosestHitPass)
	BDT_CLASS_PTR(RTBVHSecondaryRayPass)
	BDT_CLASS_PTR(RTBVHShadingPass)
	BDT_CLASS_PTR(RTBVHImageResolvePass)
	BDT_CLASS_PTR(RTBVHOutputPass)
	BDT_CLASS_PTR(MyRTBVHOptimized)

	class MyRTBVHOptimized : public IRenderPipeline
	{
		BDT_DISABLE_COPY(MyRTBVHOptimized)
		BDT_DISABLE_MOVE(MyRTBVHOptimized)
		using Super = IRenderPipeline;

	public:
		virtual ~MyRTBVHOptimized() override;
		static MyRTBVHOptimizedUPtr Create();
		virtual bool Resize(uint32 width, uint32 height) override;
		virtual void Render(const RenderScene& scene, uint32 width, uint32 height) override;
		virtual void Shutdown() override;

	private:
		MyRTBVHOptimized();
		virtual bool Init() override;

		static constexpr uint32 MaxBounces = 3;

		// Render Bus
		RTBVHBusUPtr m_bus;

		// Compute Passes
		RTBVHRayGenerationPassUPtr m_rayGenPass;
		RTBVHSkinningPassUPtr m_skinningPass;
		RTBVHRefitPassUPtr m_refitPass;
		RTBVHClosestHitPassUPtr m_closestHitPass;
		RTBVHSecondaryRayPassUPtr m_secondRayPass;
		RTBVHShadingPassUPtr m_rtShadingPass;
		RTBVHImageResolvePassUPtr m_imageResolvePass;

		// Output Pass
		RTBVHOutputPassUPtr m_outputPass;
	};
}
