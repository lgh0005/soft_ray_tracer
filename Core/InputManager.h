#pragma once
#include "Defines.h"

struct GLFWwindow;

namespace bdt
{
	BDT_CLASS_PTR(Keyboard)
	BDT_CLASS_PTR(Mouse)

	class InputManager
	{
		BDT_DECLARE_SINGLE(InputManager)

	public:
		bool Init(GLFWwindow* window);
		void Update();
		void Clear();
		void Shutdown();

	public:
		Keyboard* GetKeyboard() const { return m_keyboard.get(); }
		Mouse* GetMouse() const { return m_mouse.get(); }
		void HandleScroll(float delta);

	private:
		KeyboardUPtr m_keyboard{ nullptr };
		MouseUPtr    m_mouse{ nullptr };
	};
}
