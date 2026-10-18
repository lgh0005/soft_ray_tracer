#pragma once
#include "Defines.h"

struct GLFWwindow;

namespace bdt
{
	class IInputDevice
	{
		BDT_DISABLE_COPY(IInputDevice)
		BDT_DISABLE_MOVE(IInputDevice)

	public:
		IInputDevice(GLFWwindow* window);
		virtual ~IInputDevice();

	public:
		virtual void Update() = 0;
		virtual void Clear() = 0;

	protected:
		GLFWwindow* m_window{ nullptr };
	};
}
