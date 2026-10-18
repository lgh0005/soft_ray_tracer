#include "pch.h"
#include "IInputDevice.h"

namespace bdt
{
    IInputDevice::IInputDevice(GLFWwindow* window) : m_window(window) { }
    IInputDevice::~IInputDevice() = default;
}