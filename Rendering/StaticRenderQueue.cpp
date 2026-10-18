#include "pch.h"
#include "StaticRenderQueue.h"
#include "StaticRenderBatch.h"
#include "Rendering.h"
#include "Enums.h"
#include <Core/GLVertexArray.h>
#include <Resources/GraphicsShader.h>

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
        m_propertyBuffer = InstanceBuffer::Create();
        return m_propertyBuffer != nullptr;
    }

    void StaticRenderQueue::Clear()
    {
        for (auto& [mesh, materialMap] : m_batches)
        {
            for (auto& [material, batch] : materialMap)
                batch.Clear();
        }
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
        auto [it, inserted] = materialMap.try_emplace
        (
            material,
            mesh,
            material
        );

        it->second.Add(property);
    }

    void StaticRenderQueue::Execute(GraphicsShader* shader)
    {
        // 셰이더 로드 상태 확인
        if (!shader || !shader->IsValid()) return;

        // vao 획득 및 바인딩
        GLVertexArray* vao = BDT_RENDER.GetStaticVAO();
        if (!vao) return;

        shader->Bind();
        vao->Bind();

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

                const auto& properties = batch.GetProperties();

                // Material 바인딩
                m_renderMaterial.Bind(material);

                // 인스턴스 속성 업로드 및 바인딩
                m_propertyBuffer->Update
                (
                    properties.data(),
                    static_cast<uint32>(properties.size() * sizeof(StaticInstanceProperty))
                );
                m_propertyBuffer->Bind(static_cast<uint32>(EInstanceBinding::Property));

                // 인스턴스 드로우 콜
                glDrawElementsInstanced
                (
                    glMesh->GetPrimitive(),
                    static_cast<GLsizei>(glMesh->GetIndexCount()),
                    GL_UNSIGNED_INT,
                    nullptr,
                    static_cast<GLsizei>(instanceCount)
                );
            }
        }

        RenderMaterial::Unbind();
        InstanceBuffer::Unbind(static_cast<uint32>(EInstanceBinding::Property));
        GLVertexArray::Unbind();
    }
}
