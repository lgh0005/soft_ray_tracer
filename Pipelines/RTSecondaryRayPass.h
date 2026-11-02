#pragma once
#include <Rendering/IRenderPass.h>

namespace bdt
{
	BDT_CLASS_PTR(RTSecondaryRayPass)

	class RTSecondaryRayPass : public IRenderPass
	{
	public:
		virtual ~RTSecondaryRayPass() override;
		static RTSecondaryRayPassUPtr Create();
		virtual void Execute(IRenderBus& bus) override;
		virtual void Shutdown() override;

	private:
		RTSecondaryRayPass();
		virtual bool Init() override;

		ResourceHandle<ComputeShader> m_secondaryRayShader;
		ResourceHandle<ComputeShader> m_prepareDispatchShader;
	};
}
