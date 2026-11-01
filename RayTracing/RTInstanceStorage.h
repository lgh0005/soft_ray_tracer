#pragma once
#include "RTGeometry.h"

namespace bdt
{
    BDT_CLASS_PTR(GLStorageBuffer)
    BDT_CLASS_PTR(RTInstanceStorage)

    class RTInstanceStorage
    {
        BDT_DISABLE_COPY(RTInstanceStorage)
        BDT_DISABLE_MOVE(RTInstanceStorage)

    public:
        ~RTInstanceStorage();
        static RTInstanceStorageUPtr Create();
        bool Upload();
        void Clear();
        void Shutdown();

        bool AddInstance(const GPUInstance& instance, uint32& idx);
        bool AppendBoneMatrices(const List<mat4>& matrices, uint32& offset);
        bool AllocateSkinnedVertices(uint32 vertexCount, uint32& vertexOffset);
        
        void BindInstanceBuffer() const;
        void BindBoneBuffer() const;
        void BindSkinnedOutputBuffer() const;

        uint32 GetInstanceCount() const;
        uint32 GetBoneMatrixCount() const;
        uint32 GetSkinnedVertexCount() const;
        uint32 GetMaxSkinnedVertexCount() const;

    private:
        bool UploadInstanceBuffer();
        bool UploadBoneBuffer();
        bool ResizeSkinnedOutputBuffer();

    private:
        RTInstanceStorage();
        bool Init();

        bool m_isStorageDirty{ false };

        // CPU Instance Data
        List<GPUInstance> m_instances;
        List<mat4> m_boneMatrices;

        // GPU Skinning Output
        uint32 m_skinnedVertexCount{ 0 };
        uint32 m_maxSkinnedVertexCount{ 0 };

        // GPU Instance Storage
        GLStorageBufferUPtr m_instanceBuffer;
        GLStorageBufferUPtr m_boneBuffer;
        GLStorageBufferUPtr m_skinnedOutputBuffer;
    };
}
