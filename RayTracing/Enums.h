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

        Count
    };

	enum class ERTUniformBinding
	{
        Camera = 0,
        RTFrame,
		Count
	};
}