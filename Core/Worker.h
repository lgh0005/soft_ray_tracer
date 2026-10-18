#pragma once
#include "Defines.h"
#include "Types.h"

namespace bdt
{
	BDT_CLASS_PTR(GLSubContext)

	class Worker
	{
		BDT_DISABLE_COPY(Worker)
		BDT_DISABLE_MOVE(Worker)

	public:
		Worker(GLSubContextUPtr context);
		~Worker();

		bool Start();
		void Join();

	private:
		void Run();

	private:
		GLSubContextUPtr m_context{ nullptr };
		Thread m_thread;
	};
}

