#include "pch.h"
#include "Worker.h"
#include "Core.h"
#include "GLSubContext.h"

namespace bdt
{
    Worker::Worker(GLSubContextUPtr context)
        : m_context(std::move(context)) {}

    Worker::~Worker() { Join(); }

    bool Worker::Start()
    {
        if (m_thread.joinable()) return false;
        m_thread = Thread([this]() { Run(); });
        return m_thread.joinable();
    }

    void Worker::Join()
    {
        if (m_thread.joinable())
            m_thread.join();
    }

    void Worker::Run()
    {
        if (!m_context->AttachToCurrentThread())
            return;

        Nullable<TaskManager::TaskEntry> entry;
        while (BDT_TASK.WaitForTask(entry))
        {
            BDT_LOG_WARN
            (
                "[Worker] Task acquired (thread={})",
                GetCurrentThreadId()
            );

            bool success = entry->first();

            Action<bool> onComplete = std::move(entry->second);
            entry.reset();

            if (onComplete)
                onComplete(success);
        }

        m_context->DetachFromCurrentThread();
    }
}