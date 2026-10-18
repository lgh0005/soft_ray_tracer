#pragma once

namespace bdt
{
	// 최초 요청이면 Promise/Handle을 생성해 즉시 캐시에 넣습니다.
	// 중복 요청이면 이미 등록된 Handle을 반환합니다.
	template<typename T>
	ResourceHandle<T> ResourceManager::Reserve
	(
		const String& key,
		Cache<T>& cache,
		std::shared_ptr<Promise<std::shared_ptr<T>>>& promise
	)
	{
		if (key.empty()) return {};

		LockGuard<Mutex> lock(m_mutex);
		auto it = cache.find(key);
		if (it != cache.end()) return it->second;

		promise = std::make_shared<Promise<std::shared_ptr<T>>>();
		ResourceHandle<T> handle(promise->get_future().share());
		cache.emplace(key, handle);
		return handle;
	}

	// 일반 리소스는 Create -> 필요 시 GPU 완료 대기 -> Promise 완료
	template<typename T, typename Factory>
	ResourceHandle<T> ResourceManager::Load
	(
		const String& key,
		Cache<T>& cache,
		Factory factory,
		bool gpu
	)
	{
		std::shared_ptr<Promise<std::shared_ptr<T>>> promise;
		ResourceHandle<T> handle = Reserve(key, cache, promise);
		if (!promise) return handle;

		bool submitted = BDT_TASK.Submit
		(
			[promise, factory = std::move(factory), gpu]() mutable
			{
				std::shared_ptr<T> created = factory();
				if (created && gpu) glFinish();
				bool success = static_cast<bool>(created);
				promise->set_value(std::move(created));
				return success;
			}
		);

		if (!submitted)
			promise->set_value(nullptr);

		return handle;
	}
}