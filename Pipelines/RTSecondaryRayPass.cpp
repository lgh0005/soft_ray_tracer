#include "pch.h"
#include "RTSecondaryRayPass.h"

namespace bdt
{
	RTSecondaryRayPassUPtr RTSecondaryRayPass::Create()
	{
		RTSecondaryRayPassUPtr result(new RTSecondaryRayPass());
		if (!result->Init()) return nullptr;
		return result;
	}

	bool RTSecondaryRayPass::Init()
	{
		return false;
	}

	void RTSecondaryRayPass::Execute(IRenderBus& bus)
	{
	}

	void RTSecondaryRayPass::Shutdown()
	{
	}
}
