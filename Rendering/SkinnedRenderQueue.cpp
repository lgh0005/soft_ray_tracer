#include "pch.h"
#include "SkinnedRenderQueue.h"
#include "SkinnedRenderBatch.h"
#include "Rendering.h"
#include "Enums.h"

namespace bdt
{
    SkinnedRenderQueue::SkinnedRenderQueue() = default;
    SkinnedRenderQueue::~SkinnedRenderQueue() = default;

    SkinnedRenderQueueUPtr SkinnedRenderQueue::Create()
    {
        SkinnedRenderQueueUPtr queue(new SkinnedRenderQueue());
        if (!queue->Init()) return nullptr;
        return queue;
    }

    bool SkinnedRenderQueue::Init()
    {
        m_propertyBuffer = GLStorageBuffer::Create();
        m_boneBuffer = GLStorageBuffer::Create();
        m_shadowPropertyBuffer = GLStorageBuffer::Create();
        return m_propertyBuffer && m_boneBuffer && m_shadowPropertyBuffer;
    }

    void SkinnedRenderQueue::Clear()
    {
        for (auto& [mesh, materialMap] : m_batches)
        {
            for (auto& [material, batch] : materialMap)
                batch.Clear();
        }

        m_properties.clear();
        m_boneMatrices.clear();
        m_shadowProperties.clear();
    }

    void SkinnedRenderQueue::Add
    (
        Mesh* mesh,
        Material* material,
        const SkinnedInstanceProperty& property,
        const List<mat4>& bones
    )
    {
        if (!mesh) return;
        auto& materialMap = m_batches[mesh];
        auto [it, inserted] = materialMap.try_emplace
        (
            material,
            mesh,
            material
        );

        it->second.Add(property, bones);
    }

    bool SkinnedRenderQueue::Prepare()
    {
        m_properties.clear();
        m_boneMatrices.clear();

        for (auto& [mesh, materialMap] : m_batches)
        {
            for (auto& [material, batch] : materialMap)
            {
                const auto& properties = batch.GetProperties();
                const auto& bones = batch.GetBoneMatrices();
                if (properties.empty() || bones.empty())
                    continue;

                batch.SetBaseInstance(static_cast<uint32>(m_properties.size()));
                const uint32 baseBoneOffset = static_cast<uint32>(m_boneMatrices.size());
            
                // Bone Matrix 통합
                m_boneMatrices.insert(m_boneMatrices.end(), bones.begin(), bones.end());
                
                // Instance Property 통합
                for (const auto& property : properties)
                {
                    SkinnedInstanceProperty instance = property;
                    instance.boneOffset += baseBoneOffset;
                    m_properties.push_back(instance);
                }
            }
        }

        if (m_properties.empty())
            return true;

        // Instance SSBO 일괄 업로드
        if (!m_propertyBuffer->Update(m_properties.data(), (uint32)m_properties.size() * sizeof(SkinnedInstanceProperty)))
            return false;

        // Bone SSBO 일괄 업로드
        if (!m_boneBuffer->Update(m_boneMatrices.data(), (uint32)m_boneMatrices.size() * sizeof(mat4)))
            return false;

        return true;
    }

    void SkinnedRenderQueue::Execute(GraphicsShader* shader)
    {
        // 셰이더 로드 상태 확인
        if (!shader || !shader->IsValid()) return;
        if (m_properties.empty()) return;

        // vao 획득 및 바인딩
        GLVertexArray* vao = BDT_RENDER.GetSkinnedVAO();
        if (!vao) return;

        shader->Bind();
        vao->Bind();
        m_propertyBuffer->Bind(static_cast<uint32>(EInstanceBinding::Property));
        m_boneBuffer->Bind(static_cast<uint32>(EInstanceBinding::Bone));

        for (auto& [mesh, materialMap] : m_batches)
        {
            // 메쉬의 vbo와 ibo를 vao에 연결
            if (!mesh) continue;
            const GLMesh* glMesh = mesh->GetGLMesh();
            if (!glMesh || !glMesh->IsValid()) continue;

            const GLBuffer* vbo = glMesh->GetVertexBuffer();
            const GLBuffer* ibo = glMesh->GetIndexBuffer();
            if (!vbo || !ibo) continue;

            vao->SetVertexBuffer(*vbo, 0, sizeof(GLSkinnedVertex));
            vao->SetIndexBuffer(*ibo);

            // 배치 단위 인스턴싱 드로우콜
            for (auto& [material, batch] : materialMap)
            {
                const uint32 instanceCount = batch.GetInstanceCount();
                if (instanceCount == 0) continue;

                const auto& properties = batch.GetProperties();
                const auto& bones = batch.GetBoneMatrices();
                if (bones.empty()) continue;

                // Material 바인딩
                m_renderMaterial.Bind(material);

                // 인스턴스 드로우 콜
                glDrawElementsInstancedBaseInstance
                (
                    glMesh->GetPrimitive(),
                    static_cast<GLsizei>(glMesh->GetIndexCount()),
                    GL_UNSIGNED_INT,
                    nullptr,
                    static_cast<GLsizei>(instanceCount),
                    batch.GetBaseInstance()
                );
            }
        }

        // 언바인딩
        RenderMaterial::Unbind();
        GLStorageBuffer::Unbind(static_cast<uint32>(EInstanceBinding::Property));
        GLStorageBuffer::Unbind(static_cast<uint32>(EInstanceBinding::Bone));
        GLVertexArray::Unbind();
    }

    bool SkinnedRenderQueue::PrepareShadow(const List<ShadowData>& shadows)
    {
        m_shadowProperties.clear();
        m_shadowProperties.reserve(m_properties.size() * shadows.size());

        for (auto& [mesh, materialMap] : m_batches)
        {
            for (auto& [material, batch] : materialMap)
            {
                // Shadow Instance 시작 위치
                batch.SetShadowBaseInstance(static_cast<uint32>(m_shadowProperties.size()));
                batch.SetShadowInstanceCount(0);

                const uint32 instanceCount = batch.GetInstanceCount();
                if (instanceCount == 0) continue;

                // Mesh Instance와 Shadow 조합 생성
                for (uint32 instanceIndex = 0; instanceIndex < instanceCount; ++instanceIndex)
                {
                    for (uint32 shadowIndex = 0; shadowIndex < static_cast<uint32>(shadows.size()); ++shadowIndex)
                    {
                        ShadowInstanceProperty property{};
                        property.instanceIndex = batch.GetBaseInstance() + instanceIndex;
                        property.shadowIndex = shadowIndex;
                        m_shadowProperties.push_back(property);
                    }
                }

                // 해당 Batch의 Shadow Instance 개수
                const uint32 shadowInstanceCount =(uint32)m_shadowProperties.size() - batch.GetShadowBaseInstance();
                batch.SetShadowInstanceCount(shadowInstanceCount);
            }
        }

        // Shadow Instance가 없다면 업로드 생략
        if (m_shadowProperties.empty())
            return true;

        // Shadow Instance SSBO 업로드
        if (!m_shadowPropertyBuffer->Update(m_shadowProperties.data(), (uint32)m_shadowProperties.size() * sizeof(ShadowInstanceProperty)))
            return false;

        return true;
    }


    void SkinnedRenderQueue::ExecuteShadow(GraphicsShader* shader)
    {
        // Shadow Shader 확인
        if (!shader || !shader->IsValid()) return;
        if (m_shadowProperties.empty()) return;

        // Skinned VAO 획득
        GLVertexArray* vao = BDT_RENDER.GetSkinnedVAO();
        if (!vao) return;

        shader->Bind();
        vao->Bind();

        // 기존 Instance / Bone SSBO 바인딩
        m_propertyBuffer->Bind(static_cast<uint32>(EInstanceBinding::Property));
        m_boneBuffer->Bind(static_cast<uint32>(EInstanceBinding::Bone));

        // Shadow Instance SSBO 바인딩
        m_shadowPropertyBuffer->Bind(static_cast<uint32>(EInstanceBinding::ShadowProperty));

        for (auto& [mesh, materialMap] : m_batches)
        {
            if (!mesh) continue;

            const GLMesh* glMesh = mesh->GetGLMesh();
            if (!glMesh || !glMesh->IsValid()) continue;

            const GLBuffer* vbo = glMesh->GetVertexBuffer();
            const GLBuffer* ibo = glMesh->GetIndexBuffer();
            if (!vbo || !ibo) continue;

            vao->SetVertexBuffer(*vbo, 0, sizeof(GLSkinnedVertex));
            vao->SetIndexBuffer(*ibo);

            for (auto& [material, batch] : materialMap)
            {
                const uint32 instanceCount = batch.GetShadowInstanceCount();
                if (instanceCount == 0) continue;

                // Shadow Instancing
                glDrawElementsInstancedBaseInstance
                (
                    glMesh->GetPrimitive(),
                    static_cast<GLsizei>(glMesh->GetIndexCount()),
                    GL_UNSIGNED_INT,
                    nullptr,
                    static_cast<GLsizei>(instanceCount),
                    batch.GetShadowBaseInstance()
                );
            }
        }

        // 언바인딩
        GLStorageBuffer::Unbind(static_cast<uint32>(EInstanceBinding::ShadowProperty));
        GLStorageBuffer::Unbind(static_cast<uint32>(EInstanceBinding::Bone));
        GLStorageBuffer::Unbind(static_cast<uint32>(EInstanceBinding::Property));
        GLVertexArray::Unbind();
    }

}
