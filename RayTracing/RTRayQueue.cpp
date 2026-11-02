#include "pch.h"
#include "RTRayQueue.h"
#include "RTData.h"
#include "RTGeometry.h"
#include "Enums.h"

namespace bdt
{
    RTRayQueue::RTRayQueue() = default;

    RTRayQueue::~RTRayQueue()
    {
        Shutdown();
    }

    RTRayQueueUPtr RTRayQueue::Create()
    {
        RTRayQueueUPtr queue(new RTRayQueue());
        if (!queue->Init()) return nullptr;
        return queue;
    }

    bool RTRayQueue::Init()
    {
        // Ray Queues
        m_rayBuffer = GLStorageBuffer::Create();
        m_nextRayBuffer = GLStorageBuffer::Create();
		if (!m_rayBuffer || !m_nextRayBuffer)
			return false;

        // Hit Results
        m_hitBuffer = GLStorageBuffer::Create();
		if (!m_hitBuffer) return false;

        // Queue Counters
        m_rayCounterBuffer = GLStorageBuffer::Create();
        m_nextRayCounterBuffer = GLStorageBuffer::Create();
		if (!m_rayCounterBuffer || !m_nextRayCounterBuffer) return false;

        // Pixel Accumulation
        m_accumulationBuffer = GLStorageBuffer::Create();
        if (!m_accumulationBuffer) return false;

        // Counter 초기화
        const uint32 zero = 0;
        if (!m_rayCounterBuffer->Update(&zero, sizeof(uint32))) return false;
        if (!m_nextRayCounterBuffer->Update(&zero, sizeof(uint32))) return false;

        // Indirect Dispatch Buffer
        const GPUDispatchCommand command{ 0, 1, 1 };

        glCreateBuffers(1, &m_dispatchCommandBuffer);
        if (m_dispatchCommandBuffer == 0) return false;
        glNamedBufferStorage
        (
            m_dispatchCommandBuffer,
            sizeof(GPUDispatchCommand),
            &command,
            GL_DYNAMIC_STORAGE_BIT
        );

        return true;
    }


    bool RTRayQueue::Resize(uint32 maxRayCount, uint32 pixelCount)
    {
        if (maxRayCount == 0 || pixelCount == 0)
            return false;

        // Ray / Hit Buffer
        if (maxRayCount > m_maxRayCount)
        {
            const uint64 rayBufferSize = static_cast<uint64>(maxRayCount) * sizeof(GPURayWorkItem);
            const uint64 hitBufferSize = static_cast<uint64>(maxRayCount) * sizeof(GPUHit);
            if (rayBufferSize > MaxValue<uint32> || hitBufferSize > MaxValue<uint32>) return false;
            if (!m_rayBuffer->Resize(static_cast<uint32>(rayBufferSize))) return false;
            if (!m_nextRayBuffer->Resize(static_cast<uint32>(rayBufferSize))) return false;
            if (!m_hitBuffer->Resize(static_cast<uint32>(hitBufferSize))) return false;
            m_maxRayCount = maxRayCount;
        }

        // Pixel Accumulation
        if (pixelCount > m_maxPixelCount)
        {
            const uint64 accumulationBufferSize = static_cast<uint64>(pixelCount) * sizeof(GPUAccumulatedPixel);
            if (accumulationBufferSize > MaxValue<uint32>) return false;
            if (!m_accumulationBuffer->Resize(static_cast<uint32>(accumulationBufferSize))) return false;
            m_maxPixelCount = pixelCount;
        }

        return true;
    }


    void RTRayQueue::Clear()
    {
        const uint32 zero = 0;

        // Ray Counters
        m_rayCounterBuffer->Update(&zero, sizeof(uint32));
        m_nextRayCounterBuffer->Update(&zero, sizeof(uint32));

        // Pixel Accumulation
        ClearAccumulation();

        // Indirect Dispatch
        if (m_dispatchCommandBuffer != 0)
        {
            const GPUDispatchCommand command{ 0, 1, 1 };
            glNamedBufferSubData
            (
                m_dispatchCommandBuffer,
                0,
                sizeof(GPUDispatchCommand),
                &command
            );
        }
    }

    void RTRayQueue::Shutdown()
    {
        m_rayBuffer.reset();
        m_nextRayBuffer.reset();
        m_hitBuffer.reset();

        m_rayCounterBuffer.reset();
        m_nextRayCounterBuffer.reset();
        m_accumulationBuffer.reset();

        if (m_dispatchCommandBuffer != 0)
        {
            glDeleteBuffers(1, &m_dispatchCommandBuffer);
            m_dispatchCommandBuffer = 0;
        }

        m_maxRayCount = 0;
        m_maxPixelCount = 0;
    }

    /*==================================//
    //             Ray Queue            //
    //==================================*/
    void RTRayQueue::SwapQueues()
    {
        std::swap(m_rayBuffer, m_nextRayBuffer);
        std::swap(m_rayCounterBuffer, m_nextRayCounterBuffer);
    }

    void RTRayQueue::BindRayBuffer() const
    {
        m_rayBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Ray));
    }

    void RTRayQueue::BindNextRayBuffer() const
    {
        m_nextRayBuffer->Bind(static_cast<uint32>(ERTStorageBinding::NextRay));
    }

    void RTRayQueue::BindHitBuffer() const
    {
        m_hitBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Hit));
    }

    GLStorageBuffer* RTRayQueue::GetRayBuffer() const
    {
        return m_rayBuffer.get();
    }

    GLStorageBuffer* RTRayQueue::GetNextRayBuffer() const
    {
        return m_nextRayBuffer.get();
    }

    GLStorageBuffer* RTRayQueue::GetHitBuffer() const
    {
        return m_hitBuffer.get();
    }

    /*==================================//
    //           Ray Counters           //
    //==================================*/
    bool RTRayQueue::SetInitialRayCount(uint32 count)
    {
        if (count > m_maxRayCount) return false;
        return m_rayCounterBuffer->Update(&count, sizeof(uint32));
    }

    bool RTRayQueue::ResetNextRayCounter()
    {
        const uint32 zero = 0;
        return m_nextRayCounterBuffer->Update(&zero, sizeof(uint32));
    }

    uint32 RTRayQueue::GetMaxRayCount() const
    {
        return m_maxRayCount;
    }

    void RTRayQueue::BindRayCounter() const
    {
        m_rayCounterBuffer->Bind(static_cast<uint32>(ERTStorageBinding::RayCounter));
    }

    void RTRayQueue::BindNextRayCounter() const
    {
        m_nextRayCounterBuffer->Bind(static_cast<uint32>(ERTStorageBinding::NextRayCounter));
    }

    /*==================================//
    //         Indirect Dispatch        //
    //==================================*/
    bool RTRayQueue::SetInitialDispatch(uint32 rayCount)
    {
        if (rayCount > m_maxRayCount || m_dispatchCommandBuffer == 0) return false;
        const uint32 groupCount = static_cast<uint32>((static_cast<uint64>(rayCount) + 64 - 1) / 64);
        const GPUDispatchCommand command{ groupCount, 1, 1 };
        glNamedBufferSubData(m_dispatchCommandBuffer, 0, sizeof(GPUDispatchCommand), &command);
        return true;
    }

    void RTRayQueue::BindDispatchCommandBuffer() const
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, static_cast<uint32>(ERTStorageBinding::DispatchCommand), m_dispatchCommandBuffer);
        glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, m_dispatchCommandBuffer);
    }

    void RTRayQueue::UnbindDispatchCommandBuffer() const
    {
        glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
    }

    /*==================================//
    //        Pixel Accumulation        //
    //==================================*/
    void RTRayQueue::ClearAccumulation()
    {
        if (!m_accumulationBuffer || m_accumulationBuffer->GetSize() == 0)
            return;

        const uint32 zero = 0;
        BindAccumulationBuffer();

        // GPU Buffer를 직접 0으로 초기화
        glClearBufferData
        (
            GL_SHADER_STORAGE_BUFFER,
            GL_R32UI,
            GL_RED_INTEGER,
            GL_UNSIGNED_INT,
            &zero
        );
    }

    void RTRayQueue::BindAccumulationBuffer() const
    {
        m_accumulationBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Accumulation));
    }

    GLStorageBuffer* RTRayQueue::GetAccumulationBuffer() const
    {
        return m_accumulationBuffer.get();
    }

}