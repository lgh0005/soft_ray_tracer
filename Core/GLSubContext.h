#pragma once
#include "Defines.h"
#include "GLContext.h"

namespace bdt
{
	BDT_CLASS_PTR(GLMainContext)
	BDT_CLASS_PTR(GLSubContext)

    class GLSubContext final : public GLContext
    {
        BDT_DISABLE_COPY(GLSubContext)
        BDT_DISABLE_MOVE(GLSubContext)

    public:
        static GLSubContextUPtr Create(const GLMainContext& mainContext);
        ~GLSubContext() override;

        // 워커 스레드에서 호출
        bool AttachToCurrentThread();
        void DetachFromCurrentThread();

        // 워커 종료 후 메인 스레드에서 소멸
        void Destroy();
        GLFWwindow* GetHandle() const override;

    private:
        GLSubContext();
        bool Init(const GLMainContext& mainContext);

        GLFWwindow* m_handle{ nullptr };

        // 스레드별 비소유 포인터.
        static thread_local GLSubContext* s_current;
    };
}
