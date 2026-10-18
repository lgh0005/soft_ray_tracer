#include "pch.h"
#include "TaskManager.h"
#include "Worker.h"
#include "GLSubContext.h"
#include "GLMainContext.h"

namespace bdt
{
	TaskManager::TaskManager() = default;
	TaskManager::~TaskManager() = default;

	bool TaskManager::Init
	(
		const GLMainContext& mainContext,
		uint32 workerCount
	)
	{
		if (workerCount == 0) return false;

		m_workers.reserve(workerCount);

		// Worker별 OpenGL 공유 컨텍스트 생성
		for (uint32 i = 0; i < workerCount; ++i)
		{
			GLSubContextUPtr context = GLSubContext::Create(mainContext);
			if (!context) return false;
			m_workers.push_back(WorkerUPtr(new Worker(std::move(context))));
		}

		return true;
	}

	bool TaskManager::Launch()
	{
		if (m_workers.empty()) return false;

		for (auto& worker : m_workers)
		{
			if (!worker->Start())
				return false;
		}

		return true;
	}

	bool TaskManager::Submit(Task task, Action<bool> onComplete)
	{
		{
			LockGuard<Mutex> lock(m_mutex);
			m_tasks.emplace(std::move(task), std::move(onComplete));
		}

		m_condition.notify_one();
		return true;
	}

	bool TaskManager::WaitForTask(Nullable<TaskEntry>& output)
	{
		UniqueLock<Mutex> lock(m_mutex);
		m_condition.wait(lock, [this]() { return m_shouldStop || !m_tasks.empty(); });

		// 종료 요청 후에도 큐에 남은 작업은 처리
		if (m_tasks.empty())
			return false;

		output.emplace(std::move(m_tasks.front()));
		m_tasks.pop();

		return true;
	}

	void TaskManager::Shutdown()
	{
		{
			LockGuard<Mutex> lock(m_mutex);
			m_shouldStop = true;
		}

		// 대기 중인 모든 Worker 깨우기
		m_condition.notify_all();

		// 모든 Worker 종료 대기
		for (WorkerUPtr& worker : m_workers)
			worker->Join();

		// GLFW 메인 스레드에서 GLSubContext 파괴
		m_workers.clear();
	}
}