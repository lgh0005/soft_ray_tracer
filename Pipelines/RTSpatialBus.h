#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderBus.h>

namespace bdt
{
	class RTSpatialBus : public IRenderBus
	{
	public:
		virtual bool Resize(uint32 width, uint32 height) override;
	};
}