#pragma once
#include "RTBVHStorage.h"

namespace bdt
{
    BDT_CLASS_PTR(RTBVHUpdateStorage)
    BDT_CLASS_PTR(RTBVHBuilder)

    class RTBVHBuilder
    {
        BDT_DISABLE_COPY(RTBVHBuilder)
        BDT_DISABLE_MOVE(RTBVHBuilder)

    public:
        ~RTBVHBuilder();
        static RTBVHBuilderUPtr Create(EBVHBuildMode buildMode);
        bool Prepare();
        void Shutdown();
        bool PrepareSkinnedBounds();

        RTBVHStorage* GetStorage() const;
        void MarkBLASDirty();
        void MarkTLASDirty();
        void MarkTLASRefitDirty();
        bool IsBLASRefitRequired() const;
        bool IsTLASRefitRequired() const;
        uint32 GetMaxSkinnedNodeCount() const;
        void CompleteRefit();

    private:
        RTBVHBuilder();
        bool Init(EBVHBuildMode buildMode);

        RTBVHStorageUPtr m_storage;
        RTBVHUpdateStorageUPtr m_updateStorage;
    };
}
