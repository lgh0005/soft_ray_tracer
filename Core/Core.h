#pragma once

// Common
#include "Defines.h"
#include "Types.h"

// Managers
#include "InputManager.h"
#include "TimeManager.h"
#include "WindowManager.h"
#include "TaskManager.h"
#define BDT_INPUT  bdt::InputManager::Instance()
#define BDT_TIME   bdt::TimeManager::Instance()
#define BDT_WINDOW bdt::WindowManager::Instance()
#define BDT_TASK   bdt::TaskManager::Instance()

// Devices
#include "Keyboard.h"
#include "Mouse.h"
#define BDT_KEYBOARD BDT_INPUT.GetKeyboard()
#define BDT_MOUSE    BDT_INPUT.GetMouse()

// Debug Utilities
#include "Asserter.h"
#include "Logger.h"
#include "Profiler.h"