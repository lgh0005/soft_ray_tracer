#pragma once
#include "RTBVHStorage.h"
#include "RTGeometry.h"

namespace bdt
{
    BDT_CLASS_PTR(Mesh)
    BDT_CLASS_PTR(RTBVHUpdateStorage)

    class RTBVHUpdateStorage
    {
        BDT_DISABLE_COPY(RTBVHUpdateStorage)
        BDT_DISABLE_MOVE(RTBVHUpdateStorage)

    public:
        ~RTBVHUpdateStorage();
        static RTBVHUpdateStorageUPtr Create();
        void Clear();
        void Shutdown();

        bool DetectChanges(const RTBVHStorage& storage);
        void CompleteBuild();
        bool PrepareMeshInputs();
        bool PrepareInstanceBounds();
        bool PrepareSkinnedBounds(RTBVHStorage& storage);

        const List<RTBVHMeshInput>& GetMeshInputs() const;
        const List<GPUBVHNodeBounds>& GetInstanceBounds() const;
        const List<uint32>& GetBoundsOffsets() const;

        bool IsBLASDirty() const;
        bool IsTLASDirty() const;
        bool IsSkinnedLayoutDirty() const;
        void MarkBLASDirty();
        void MarkTLASDirty();
        void MarkTLASRefitDirty();

        bool IsBLASRefitRequired() const;
        bool IsTLASRefitRequired() const;
        uint32 GetMaxSkinnedNodeCount() const;
        void CompleteRefit();

    private:
        void DetectGeometryChanges();
        bool DetectInstanceChanges();
        void DetectBoneChanges();
        void SaveSnapshot();
        GPUBVHNodeBounds TransformBounds
        (
            const GPUBVHNodeBounds& localBounds, 
            const mat4& worldMatrix
        ) const;

    private:
        RTBVHUpdateStorage();
        bool Init();

        bool m_blasDirty{ true };
        bool m_tlasDirty{ true };
        bool m_skinnedLayoutDirty{ true };
        bool m_gpuBLASRefitRequired{ false };
        bool m_gpuTLASRefitRequired{ false };
        uint32 m_maxSkinnedNodeCount{ 0 };
        List<uint32> m_boundsOffsets;

        List<MeshPtr> m_previousMeshes;
        List<GPUMeshDescriptor> m_previousDescriptors;
        List<GPUInstance> m_previousInstances;
        List<const RenderObject*> m_previousSources;
        List<mat4> m_previousBones;

        List<List<vec3>> m_vertices;
        List<List<uint32>> m_indices;
        List<GPUBVHNodeBounds> m_meshLocalBounds;
        List<RTBVHMeshInput> m_meshInputs;
        List<GPUBVHNodeBounds> m_instanceBounds;
    };
}
