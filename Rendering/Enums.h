#pragma once
#include <Core/Types.h>

namespace bdt
{
	// Instancing SSBO Binding Point
	enum class EInstanceBinding : uint8
	{
		Property,
		Bone,
		ShadowProperty = 4
	};

	// UBO Binding Point
	enum class EUniformBinding : uint8
	{
		Camera,
		Material,
		Light,
		ScreenSpace
	};

	// SSBO Bidning Point
	enum class EStorageBinding : uint8
	{
		Property,
		Bone,
		Light,
		Shadow
	};

	// Default Geometry Buffer primitives
	enum class ESurfaceTextureSlot : uint8
	{
		Position,
		Normal,
		Albedo,
		Material,
		DepthStencil,
		Count
	};

	// Simple Light Types
	enum class ELightType : uint8
	{
		Directional,
		Point,
		Spot
	};
}
