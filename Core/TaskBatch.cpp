#include "pch.h"
#include "TaskBatch.h"
#include "Core.h"

namespace bdt
{
    TaskBatch::TaskBatch(List<Task> tasks, Action<bool> onComplete)
        : m_tasks(std::move(tasks)),
        m_remaining(static_cast<int32>(m_tasks.size())),
        m_onComplete(std::move(onComplete)) {}

    bool TaskBatch::Submit()
    {
        if (m_tasks.empty()) return false;
        
        auto self = shared_from_this();
        for (usize i = 0; i < m_tasks.size(); ++i)
        {
            BDT_TASK.Submit
            (
                [self, i]() { return self->m_tasks[i](); },
                [self](bool success) { self->Complete(success); }
            );
        }

        return true;
    }

    void TaskBatch::Complete(bool success)
    {
        if (!success) m_success.store(false);

        // 가장 마지막에 실행된 스레드에서 OnComplete를 호출.
        if (m_remaining.fetch_sub(1) == 1)
        {
            if (m_onComplete)
                m_onComplete(m_success.load());
        }
    }
}