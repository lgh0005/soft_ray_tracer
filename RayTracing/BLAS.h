#pragma once
#include "BVH.h"

namespace bdt
{
    class BLAS final : public BVH
    {
        BDT_DISABLE_COPY(BLAS);
        BDT_DISABLE_MOVE(BLAS);
		using Super = BVH;

    public:
        explicit BLAS(uint32 meshIndex, EBVHBuildMode buildMode);
        virtual ~BLAS() override;

        bool Build
        (
            const List<vec3>& vertices,
            const List<uint32>& indices,
            uint32 maxLeafPrimitives = 4
        );

        bool Rebuild
        (
            const List<vec3>& vertices,
            const List<uint32>& indices,
            uint32 maxLeafPrimitives = 4
        );

        bool Refit(const List<vec3>& vertices, const List<uint32>& indices);

        uint32 GetMeshIndex() const;

    private:
        bool CalculateTriangleBounds
        (
            const List<vec3>& vertices,
            const List<uint32>& indices,
            List<GPUBVHNodeBounds>& triangleBounds
        ) const;

    private:
        uint32 m_meshIndex{ 0 };
    };
}
