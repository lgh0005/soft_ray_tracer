#pragma once
#include <Rendering/IRenderPass.h>

namespace bdt
{
	BDT_CLASS_PTR(ComputeShader)
	BDT_CLASS_PTR(RTRayGenerationPass)

	class RTRayGenerationPass : public IRenderPass
	{
	public:
		virtual ~RTRayGenerationPass() override;
		static RTRayGenerationPassUPtr Create();
		virtual void Execute(IRenderBus& bus) override;
		virtual void Shutdown() override;

	private:
		RTRayGenerationPass();
		virtual bool Init() override;

		ResourceHandle<ComputeShader> m_rayGenerationShader;
	};
}
