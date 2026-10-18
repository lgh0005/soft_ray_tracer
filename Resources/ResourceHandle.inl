#pragma once

namespace bdt
{
	template<typename T>
	ResourceHandle<T>::ResourceHandle() = default;

    template<typename T>
    ResourceHandle<T>::ResourceHandle(SharedFuture<std::shared_ptr<T>> future)
        : m_future(std::move(future)) { }

    template<typename T>
    bool ResourceHandle<T>::IsReady() const
    {
        return TryWait();
    }

    template<typename T>
    std::shared_ptr<T> ResourceHandle<T>::Get() const
    {
        if (!TryWait()) return nullptr;
        return m_future.get();
    }

    template<typename T>
    bool ResourceHandle<T>::TryWait() const
    {
        if (!m_future.valid()) return false;
        return m_future.wait_for(Milliseconds(0)) == std::future_status::ready;
    }
}