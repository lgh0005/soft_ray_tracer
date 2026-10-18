#pragma once
#include <Core/Types.h>

namespace bdt
{
    template<typename T>
    class ResourceHandle
    {
    public:
        ResourceHandle();
        explicit ResourceHandle(SharedFuture<std::shared_ptr<T>> future);
        // 완료 여부만 조회한다. 로딩 성공 여부는 Get()으로 확인한다.
        bool IsReady() const;
        std::shared_ptr<T> Get() const;

    private:
        bool TryWait() const;

    private:
        SharedFuture<std::shared_ptr<T>> m_future;
    };
}

#include "ResourceHandle.inl"