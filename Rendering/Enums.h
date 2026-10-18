#pragma once
#include <Core/Types.h>

namespace bdt
{
	// Instancing SSBO Binding Point
	enum class EInstanceBinding : uint8
	{
		Property,
		Bone
	};

	// UBO Binding Point
	enum class EUniformBinding : uint8
	{
		Camera,
		Material,
		Light
	};

	// SSBO Bidning Point
	enum class EStorageBinding : uint8
	{
		Property,
		Bone,
		Light
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
}