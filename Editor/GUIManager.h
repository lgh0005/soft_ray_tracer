#pragma once
#include <Core/Defines.h>

struct ImGuiContext;

namespace bdt
{
	class GUIManager
	{
		BDT_DECLARE_SINGLE(GUIManager)

	public:
		bool Init();
		void Render();
		void Shutdown();

	private:
		ImGuiContext* m_context{ nullptr };

		// TODO : 렌더링할 패널 들을 여기에 등록한다.
	};
}

