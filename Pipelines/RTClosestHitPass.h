#pragma once
#include <Rendering/IRenderPass.h>

namespace bdt
{
	BDT_CLASS_PTR(RTClosestHitPass)

	class RTClosestHitPass : public IRenderPass
	{
	public:
		virtual  ~RTClosestHitPass() override;
		static RTClosestHitPassUPtr Create();
		virtual void Execute(IRenderBus& bus) override;
		virtual void Shutdown() override;

	private:
		RTClosestHitPass();
		virtual bool Init() override;
		ResourceHandle<ComputeShader> m_closestHitShader;
	};
}
