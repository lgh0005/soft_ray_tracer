#pragma once

namespace bdt
{
    enum class ERTStorageBinding
    {
        // Ray Tracing
        Ray = 0,
        Hit,

        // Geometry
        StaticVertex,
        Triangle,
        Mesh,
        Instance,
        SkinnedVertex,
        Bone,
        SkinnedOutput,

        // Ray Queue
        NextRay,
        RayCounter,
        NextRayCounter,
        DispatchCommand,
        Accumulation,

        Count
    };

	enum class ERTUniformBinding
	{
        Camera = 0,
        RTFrame,
		Count
	};
}