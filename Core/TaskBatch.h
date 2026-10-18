#pragma once
#include "Types.h"

namespace bdt
{
	class TaskBatch : public Self<TaskBatch>
	{
		using Task = Func<bool>;

	public:
		TaskBatch(List<Task> tasks, Action<bool> onComplete);
		bool Submit();

	private:
		void Complete(bool success);

	private:
		List<Task> m_tasks;
		Atomic<int32> m_remaining{ 0 };
		Atomic<bool> m_success{ true };
		Action<bool> m_onComplete;
	};
}

