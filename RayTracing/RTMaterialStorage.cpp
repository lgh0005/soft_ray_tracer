#include "pch.h"
#include "RTMaterialStorage.h"
#include "RTMaterial.h"
#include "Enums.h"
#include <Core/GLStorageBuffer.h>
#include <Core/GLSampler.h>
#include <Resources/Material.h>

namespace bdt
{
    RTMaterialStorage::RTMaterialStorage() = default;

    RTMaterialStorage::~RTMaterialStorage()
    {
        Shutdown();
    }

    RTMaterialStorageUPtr RTMaterialStorage::Create()
    {
        RTMaterialStorageUPtr storage(new RTMaterialStorage());
        if (!storage->Init()) return nullptr;
        return storage;
    }

    bool RTMaterialStorage::Init()
    {
        m_materialBuffer = GLStorageBuffer::Create();
        m_materialSampler = GLSampler::Create();
        if (!m_materialBuffer || !m_materialSampler) return false;
        m_materialSampler->SetFilter(GL_LINEAR, GL_LINEAR);
        m_materialSampler->SetWrap(GL_REPEAT, GL_REPEAT);
        Clear();
        return true;
    }

    bool RTMaterialStorage::Upload()
    {
        if (!m_materialBuffer) return false;

        // 공통 슬롯 배정용 임시 목록이며 텍스처 핸들을 Storage 멤버로 중복 보관하지 않는다.
        List<ResourceHandle<Image2D>> textures;
        for (usize i = 0; i < m_materials.size(); ++i)
        {
            if (!m_materials[i].Update(textures)) continue;
            m_gpuMaterials[i] = m_materials[i].GetData();
        }

        const uint64 bytes = static_cast<uint64>(m_gpuMaterials.size()) * sizeof(GPURTMaterial);
        if (bytes > MaxValue<uint32>) return false;
        return m_materialBuffer->Update(m_gpuMaterials.data(), static_cast<uint32>(bytes));
    }

    void RTMaterialStorage::Clear()
    {
        ResetMaterialIndices();
        m_gpuMaterials.clear();
        m_materials.clear();

        m_materials.emplace_back();
        List<ResourceHandle<Image2D>> textures;
        m_materials.back().Update(textures);
        m_gpuMaterials.push_back(m_materials.back().GetData());
    }

    void RTMaterialStorage::Shutdown()
    {
        ResetMaterialIndices();
        m_materialBuffer.reset();
        m_materialSampler.reset();
        m_gpuMaterials.clear();
        m_materials.clear();
    }

    uint32 RTMaterialStorage::RegisterMaterial(const ResourceHandle<Material>& handle)
    {
        if (!m_materialBuffer || m_gpuMaterials.empty()) return MaxValue<uint32>;
        const auto material = handle.Get();
        if (!material) return 0;

        // Material이 보관하는 슬롯이 현재 Storage의 원본과 일치하는지 확인
        uint32 index = material->GetIndex();
        if (index < m_materials.size())
        {
            if (m_materials[index].GetMaterial().Get() == material) 
                return index;
        }

        // 다른 Storage 사용 또는 외부 인덱스 변경 후에도 기존 슬롯을 복구
        for (uint32 i = 1; i < m_materials.size(); ++i)
        {
            if (m_materials[i].GetMaterial().Get() != material) continue;
            index = i;
            material->SetIndex(index);
            return index;
        }

        if (m_gpuMaterials.size() >= MaxValue<uint32> / sizeof(GPURTMaterial)) return MaxValue<uint32>;

        index = static_cast<uint32>(m_gpuMaterials.size());
        m_materials.emplace_back(handle);
        m_gpuMaterials.push_back(m_materials.back().GetData());
        material->SetIndex(index);

        return index;
    }

    void RTMaterialStorage::ResetMaterialIndices()
    {
        for (uint32 i = 1; i < m_materials.size(); ++i)
        {
            const auto material = m_materials[i].GetMaterial().Get();
            if (!material) continue;
            if (material->GetIndex() == i) material->SetIndex(MaxValue<uint32>);
        }
    }

    void RTMaterialStorage::Bind() const
    {
        m_materialBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Material));
        for (uint32 i = 0; i < 16u; ++i)
        {
            // 이전 패스의 미사용 텍스처 슬롯이 남지 않도록 초기화
            glBindTextureUnit(i, 0);
            m_materialSampler->Bind(i);
        }

        for (const RTMaterial& material : m_materials)
            material.Bind();
    }

    uint32 RTMaterialStorage::GetMaterialCount() const
    {
        return static_cast<uint32>(m_gpuMaterials.size());
    }
}
