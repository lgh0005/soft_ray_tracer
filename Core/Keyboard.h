#pragma once
#include "IInputDevice.h"
#include "Defines.h"
#include "Enums.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

// GLFW가 임시로 정의한 호출 규약 매크로만 정리해 Windows 헤더와의 재정의를 방지한다.
#if defined(_WIN32) && defined(GLFW_APIENTRY_DEFINED)
#ifdef GLFW_GLAPIENTRY_DEFINED
#undef GLAPIENTRY
#undef GLFW_GLAPIENTRY_DEFINED
#endif
#undef APIENTRY
#undef GLFW_APIENTRY_DEFINED
#endif

namespace bdt
{
	class Keyboard : public IInputDevice
	{
		BDT_DISABLE_COPY(Keyboard)
		BDT_DISABLE_MOVE(Keyboard)
		using Super = IInputDevice;

	public:
		explicit Keyboard(GLFWwindow* window);
		virtual ~Keyboard() override;

	public:
		void Update() override;
		void Clear() override;

	public:
		bool GetKeyDown(int32 key) const;
		bool GetKeyPress(int32 key) const;
		bool GetKeyUp(int32 key) const;
		bool GetKeyHold(int32 key) const;
		EInputState GetKeyState(int32 key) const;

	private:
		Array<EInputState, GLFW_KEY_LAST + 1> m_states{};
	};
}
