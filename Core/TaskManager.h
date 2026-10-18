#pragma once
#include "Defines.h"
#include "Types.h"

namespace bdt
{
	BDT_CLASS_PTR(Worker)
	BDT_CLASS_PTR(GLMainContext)

	class TaskManager
	{
		BDT_DECLARE_SINGLE(TaskManager)
		using Task = Func<bool>;
		using TaskEntry = Pair<Task, Action<bool>>;
		friend class Worker;

	public:
		bool Init(const GLMainContext& mainContext, uint32 workerCount);
		bool Launch();
		void Shutdown();
		bool Submit(Task task, Action<bool> onComplete = {});

	private:
		bool WaitForTask(Nullable<TaskEntry>& output);

	private:
		List<WorkerUPtr> m_workers;
		Queue<TaskEntry> m_tasks;

		Mutex m_mutex;
		ConditionVariable m_condition;

		bool m_shouldStop{ false };
	};
}

