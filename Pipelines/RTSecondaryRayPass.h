#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>

namespace bdt
{
	BDT_CLASS_PTR(RTSecondaryRayPass)

	class RTSecondaryRayPass : public IRenderPass
	{
	public:
		static RTSecondaryRayPassUPtr Create();
		virtual bool Init() override;
		virtual void Execute(IRenderBus& bus) override;
		virtual void Shutdown() override;
	};
}
