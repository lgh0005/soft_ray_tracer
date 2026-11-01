#include "pch.h"
#include "StaticRenderQueue.h"
#include "StaticRenderBatch.h"
#include "Rendering.h"
#include "Enums.h"

namespace bdt
{
    StaticRenderQueue::StaticRenderQueue() = default;
    StaticRenderQueue::~StaticRenderQueue() = default;

    StaticRenderQueueUPtr StaticRenderQueue::Create()
    {
        StaticRenderQueueUPtr queue(new StaticRenderQueue());
        if (!queue->Init()) return nullptr;
        return queue;
    }

    bool StaticRenderQueue::Init()
    {
        m_propertyBuffer = GLStorageBuffer::Create();
        m_shadowPropertyBuffer = GLStorageBuffer::Create();
        return m_propertyBuffer && m_shadowPropertyBuffer;
    }

    void StaticRenderQueue::Clear()
    {
        for (auto& [mesh, materialMap] : m_batches)
        {
            for (auto& [material, batch] : materialMap)
                batch.Clear();
        }

        m_properties.clear();
        m_shadowProperties.clear();
    }

    void StaticRenderQueue::Add
    (
        Mesh* mesh,
        Material* material,
        const StaticInstanceProperty& property
    )
    {
        if (!mesh) return;
        auto& materialMap = m_batches[mesh];
        auto [it, inserted] = materialMap.try_emplace(material, mesh, material);
        it->second.Add(property);
    }

    bool StaticRenderQueue::Prepare()
    {
        m_properties.clear();
        for (auto& [mesh, materialMap] : m_batches)
        {
            for (auto& [material, batch] : materialMap)
            {
                const auto& properties = batch.GetProperties();
                if (properties.empty()) continue;
                batch.SetBaseInstance(static_cast<uint32>(m_properties.size()));
                m_properties.insert(m_properties.end(), properties.begin(), properties.end());
            }
        }

        if (m_properties.empty())
            return true;

        if (!m_propertyBuffer->Update(m_properties.data(), (uint32)m_properties.size() * sizeof(StaticInstanceProperty)))
            return false;

        return true;
    }

    void StaticRenderQueue::Execute(GraphicsShader* shader)
    {
        // 셰이더 로드 상태 확인
        if (!shader || !shader->IsValid()) return;
        if (m_properties.empty()) return;

        // vao 획득 및 바인딩
        GLVertexArray* vao = BDT_RENDER.GetStaticVAO();
        if (!vao) return;

        shader->Bind();
        vao->Bind();
        m_propertyBuffer->Bind(static_cast<uint32>(EInstanceBinding::Property));

        for (auto& [mesh, materialMap] : m_batches)
        {
            // 메쉬의 vbo와 ibo를 vao에 연결
            if (!mesh) continue;

            const GLMesh* glMesh = mesh->GetGLMesh();
            if (!glMesh || !glMesh->IsValid())  continue;

            const GLBuffer* vbo = glMesh->GetVertexBuffer();
            const GLBuffer* ibo = glMesh->GetIndexBuffer();
            if (!vbo || !ibo) continue;

            vao->SetVertexBuffer(*vbo, 0, sizeof(GLStaticVertex));
            vao->SetIndexBuffer(*ibo);

            // 배치 단위 인스턴싱 드로우콜
            for (auto& [material, batch] : materialMap)
            {
                const uint32 instanceCount = batch.GetInstanceCount();
                if (instanceCount == 0) continue;

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

        RenderMaterial::Unbind();
        GLStorageBuffer::Unbind(static_cast<uint32>(EInstanceBinding::Property));
        GLVertexArray::Unbind();
    }

    bool StaticRenderQueue::PrepareShadow(const List<ShadowData>& shadows)
    {
        m_shadowProperties.clear();

        for (auto& [mesh, materialMap] : m_batches)
        {
            for (auto& [material, batch] : materialMap)
            {
                // Shadow Instance 시작 위치
                batch.SetShadowBaseInstance(static_cast<uint32>(m_shadowProperties.size()));
                batch.SetShadowInstanceCount(0);

                const uint32 instanceCount = batch.GetInstanceCount();
                if (instanceCount == 0) continue;

                // Mesh Instance와 Shadow의 조합 생성
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

                // 해당 배치의 Shadow Instance 개수
                const uint32 shadowInstanceCount = (uint32)m_shadowProperties.size() - batch.GetShadowBaseInstance();
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


    void StaticRenderQueue::ExecuteShadow(GraphicsShader* shader)
    {
        // Shader 및 Shadow Instance 확인
        if (!shader || !shader->IsValid()) return;
        if (m_shadowProperties.empty()) return;

        // Static VAO 획득
        GLVertexArray* vao = BDT_RENDER.GetStaticVAO();
        if (!vao) return;

        shader->Bind();
        vao->Bind();

        // 기존 Instance SSBO
        m_propertyBuffer->Bind(static_cast<uint32>(EInstanceBinding::Property));

        // Shadow Instance SSBO
        m_shadowPropertyBuffer->Bind(static_cast<uint32>(EInstanceBinding::ShadowProperty));

        for (auto& [mesh, materialMap] : m_batches)
        {
            if (!mesh) continue;

            const GLMesh* glMesh = mesh->GetGLMesh();
            if (!glMesh || !glMesh->IsValid()) continue;

            const GLBuffer* vbo = glMesh->GetVertexBuffer();
            const GLBuffer* ibo = glMesh->GetIndexBuffer();
            if (!vbo || !ibo) continue;

            vao->SetVertexBuffer(*vbo, 0, sizeof(GLStaticVertex));
            vao->SetIndexBuffer(*ibo);

            for (auto& [material, batch] : materialMap)
            {
                const uint32 instanceCount = batch.GetShadowInstanceCount();
                if (instanceCount == 0) continue;

                // Shadow 전용 Instanced Draw
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

        GLStorageBuffer::Unbind(static_cast<uint32>(EInstanceBinding::ShadowProperty));
        GLStorageBuffer::Unbind(static_cast<uint32>(EInstanceBinding::Property));
        GLVertexArray::Unbind();
    }

}
