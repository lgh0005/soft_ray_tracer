#pragma once
#include <Core/Types.h>

namespace bdt
{
    enum class ERTStorageBinding : uint8
    {
        // Ray Tracing
        Ray = 0,
        Hit,

        // Geometry
        StaticVertex = 3,
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
        Material,

        // BVH storage
        BVHTopology,
        BVHBounds,
        BVHPrimitive,
        BVHDescriptor,
        BVHSkinnedBounds,

        // Screen
        Surface,
        DirectLighting,
        AmbientOcclusion,

        Count
    };

	enum class ERTUniformBinding : uint8
	{
        Camera = 0,
        RTFrame,
        RenderSettings,
        Environment,
		Count
	};

    enum class EBVHBuildMode : uint8
    {
        MedianSplit = 0,
        SAH, // Existing 12-bin SAH
        SAH8,
        SAH16,
        SAH32
    };

    enum class ERTMeshType : uint32
    {
        Static = 0,
        Skinned
    };

    //enum class EBVHDirtyFlag : uint32
    //{
    //    None      = 0,
    //    BLAS      = 1u << 0,
    //    TLAS      = 1u << 1,
    //    TLASRefit = 1u << 2
    //};
}
