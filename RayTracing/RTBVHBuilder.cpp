#include "pch.h"
#include "RTBVHBuilder.h"
#include "RTBVHUpdateStorage.h"
#include "RayTracing.h"

namespace bdt
{
    RTBVHBuilder::RTBVHBuilder() = default;

    RTBVHBuilder::~RTBVHBuilder()
    {
        Shutdown();
    }

    RTBVHBuilderUPtr RTBVHBuilder::Create(EBVHBuildMode buildMode)
    {
        RTBVHBuilderUPtr builder(new RTBVHBuilder());
        if (!builder->Init(buildMode)) return nullptr;
        return builder;
    }

    bool RTBVHBuilder::Init(EBVHBuildMode buildMode)
    {
        m_storage = RTBVHStorage::Create(buildMode);
        if (!m_storage) return false;
        m_updateStorage = RTBVHUpdateStorage::Create();
        return m_updateStorage != nullptr;
    }

    bool RTBVHBuilder::Prepare()
    {
        if (!m_updateStorage->DetectChanges(*m_storage)) 
            return false;

        if (m_updateStorage->IsBLASDirty())
        {
            if (!m_updateStorage->PrepareMeshInputs()) return false;
            if (!m_storage->RebuildBLAS(m_updateStorage->GetMeshInputs())) return false;
        }

        if (m_updateStorage->IsSkinnedLayoutDirty() && !PrepareSkinnedBounds()) return false;
        if (!BDT_INSTANCE.UpdateBLASBoundsOffsets(m_updateStorage->GetBoundsOffsets())) return false;

        if (m_updateStorage->IsTLASDirty())
        {
            if (!m_updateStorage->PrepareInstanceBounds()) return false;
            if (!m_storage->RebuildTLAS(m_updateStorage->GetInstanceBounds())) return false;
        }

        if (!m_storage->Upload()) return false;
        m_updateStorage->CompleteBuild();
        return true;
    }

    bool RTBVHBuilder::SetBuildSettings(const RTBVHBuildSettings& settings)
    {
        if (!m_storage || !m_updateStorage) return false;
        const RTBVHBuildSettings previous = m_storage->GetBuildSettings();
        if (!m_storage->SetBuildSettings(settings)) return false;
        if (previous.blasBuildMode != settings.blasBuildMode || previous.blasMaxLeafPrimitives != settings.blasMaxLeafPrimitives)
            m_updateStorage->MarkBLASDirty();
        else if (previous.tlasBuildMode != settings.tlasBuildMode || previous.tlasMaxLeafPrimitives != settings.tlasMaxLeafPrimitives)
            m_updateStorage->MarkTLASDirty();
        return true;
    }

    bool RTBVHBuilder::PrepareRefit(RTBVHRefitData& data, bool parallelInternalRefit)
    {
        data = {};
        if (!m_storage || !m_updateStorage) return false;
        data.refitBLAS = m_updateStorage->IsBLASRefitRequired();
        data.refitTLAS = m_updateStorage->IsTLASRefitRequired();
        data.maxSkinnedNodeCount = m_updateStorage->GetMaxSkinnedNodeCount();
        return m_storage->PrepareRefit(data, parallelInternalRefit);
    }

    bool RTBVHBuilder::PrepareSkinnedBounds()
    {
        if (!m_storage || !m_updateStorage) return false;
        return m_updateStorage->PrepareSkinnedBounds(*m_storage);
    }

    RTBVHStorage* RTBVHBuilder::GetStorage() const
    {
        return m_storage.get();
    }

    void RTBVHBuilder::Shutdown()
    {
        m_storage.reset();
        m_updateStorage.reset();
    }

    void RTBVHBuilder::MarkBLASDirty()
    {
        if (m_updateStorage) m_updateStorage->MarkBLASDirty();
    }

    void RTBVHBuilder::MarkTLASDirty()
    {
        if (m_updateStorage) m_updateStorage->MarkTLASDirty();
    }

    void RTBVHBuilder::MarkTLASRefitDirty()
    {
        if (m_updateStorage) m_updateStorage->MarkTLASRefitDirty();
    }

    bool RTBVHBuilder::IsBLASRefitRequired() const
    {
        if (!m_updateStorage) return false;
        return m_updateStorage->IsBLASRefitRequired();
    }

    bool RTBVHBuilder::IsTLASRefitRequired() const
    {
        if (!m_updateStorage) return false;
        return m_updateStorage->IsTLASRefitRequired();
    }

    uint32 RTBVHBuilder::GetMaxSkinnedNodeCount() const
    {
        if (!m_updateStorage) return 0;
        return m_updateStorage->GetMaxSkinnedNodeCount();
    }

    void RTBVHBuilder::CompleteRefit()
    {
        if (m_updateStorage) m_updateStorage->CompleteRefit();
    }
}
