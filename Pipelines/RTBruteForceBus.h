#pragma once
#include <Rendering/IRenderBus.h>

namespace bdt
{
	class RTBruteForceBus : public IRenderBus
	{
	public:
		virtual bool Resize(uint32 width, uint32 height) override;
	};
}