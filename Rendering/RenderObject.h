#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
    BDT_CLASS_PTR(Mesh)
    BDT_CLASS_PTR(Material)

    class RenderObject
    {
    public:
        explicit RenderObject
        (
            MeshPtr mesh,
            const ResourceHandle<Material>& material,
            const mat4& worldMatrix,
            bool isSkinned = false,
            const List<mat4>* boneMatrices = nullptr
        );
        ~RenderObject();

        bool IsSkinned() const;
        const List<mat4>* GetBoneMatrices() const;
        void SetWorldMatrix(const mat4& matrix);

    public:
        const MeshPtr& GetMesh() const;
        const ResourceHandle<Material>& GetMaterial() const;
        const mat4& GetWorldMatrix() const;

    private:
        MeshPtr m_mesh{ nullptr };
        ResourceHandle<Material> m_material;
        mat4 m_worldMatrix{ 1.0f };
        bool m_isSkinned{ false };

        // 비소유 뼈 행렬 참조
        const List<mat4>* m_boneMatrices{ nullptr };
    };
}
