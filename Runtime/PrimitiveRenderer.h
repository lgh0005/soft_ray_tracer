#pragma once
#include "RootComponent.h"

namespace bdt
{
	class PrimitiveRenderer : public RootComponent
	{
		using Super = RootComponent;

	public:
		explicit PrimitiveRenderer(Actor* owner);
		~PrimitiveRenderer() override;
	};
}
