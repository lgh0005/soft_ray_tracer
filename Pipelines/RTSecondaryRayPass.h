#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>

namespace bdt
{
	class RTSecondaryRayPass : public IRenderPass
	{
	public:
		virtual bool Init() override;
		virtual void Execute(IRenderBus& bus) override;
		virtual void Shutdown() override;
	};
}
