#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>

namespace bdt
{
    BDT_CLASS_PTR(GLStorageBuffer)
    BDT_CLASS_PTR(RTRayQueue)

    class RTRayQueue
    {
        BDT_DISABLE_COPY(RTRayQueue)
        BDT_DISABLE_MOVE(RTRayQueue)

    public:
        ~RTRayQueue();
        static RTRayQueueUPtr Create();
        bool Resize(uint32 maxRayCount, uint32 pixelCount);
        void Clear();
        void Shutdown();

    /*==================================//
    //             Ray Queue            //
    //==================================*/
    public:
        void SwapQueues();

        void BindRayBuffer() const;
        void BindNextRayBuffer() const;
        void BindHitBuffer() const;
        void BindSurfaceBuffer() const;
        void BindDirectLightingBuffer() const;
        bool PrepareAmbientOcclusionBuffer();
        void BindAmbientOcclusionBuffer() const;
        bool IsAmbientOcclusionReady() const;
        bool SetAmbientOcclusionReady(bool ready);

        GLStorageBuffer* GetRayBuffer() const;
        GLStorageBuffer* GetNextRayBuffer() const;
        GLStorageBuffer* GetHitBuffer() const;
        GLStorageBuffer* GetSurfaceBuffer() const;
        GLStorageBuffer* GetDirectLightingBuffer() const;
        GLStorageBuffer* GetAmbientOcclusionBuffer() const;

    /*==================================//
    //           Ray Counters           //
    //==================================*/
    public:
        bool SetInitialRayCount(uint32 count);
        bool ResetNextRayCounter();

        // Reserved capacity and the current frame request are distinct.
        uint32 GetMaxRayCount() const;
        uint32 GetRayLimit() const;
        uint32 GetPixelCount() const;
        void BindRayCounter() const;
        void BindNextRayCounter() const;

    /*==================================//
    //         Indirect Dispatch        //
    //==================================*/
    public:
        bool SetInitialDispatch(uint32 rayCount);
        void BindDispatchCommandBuffer() const;
        void UnbindDispatchCommandBuffer() const;

    /*==================================//
    //        Pixel Accumulation        //
    //==================================*/
    public:
        void ClearAccumulation();
        void BindAccumulationBuffer() const;
        GLStorageBuffer* GetAccumulationBuffer() const;

    private:
        RTRayQueue();
        bool Init();

        uint32 m_maxRayCount{ 0 };
        uint32 m_maxPixelCount{ 0 };
        uint32 m_rayLimit{ 0 };
        uint32 m_pixelCount{ 0 };

        // Ray Queues
        GLStorageBufferUPtr m_rayBuffer;
        GLStorageBufferUPtr m_nextRayBuffer;
        GLStorageBufferUPtr m_surfaceBuffer;
        GLStorageBufferUPtr m_directLightingBuffer;
        GLStorageBufferUPtr m_ambientOcclusionBuffer;
        bool m_ambientOcclusionReady{ false };

        // Hit Results
        GLStorageBufferUPtr m_hitBuffer;

        // Queue Counters
        GLStorageBufferUPtr m_rayCounterBuffer;
        GLStorageBufferUPtr m_nextRayCounterBuffer;

        // Indirect Dispatch
        uint32 m_dispatchCommandBuffer{ 0 };

        // Pixel Accumulation
        GLStorageBufferUPtr m_accumulationBuffer;
    };
}
