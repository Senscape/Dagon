////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2013 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DAGON_PLATFORM_H_
#define DAGON_PLATFORM_H_

////////////////////////////////////////////////////////////
// Detect the current OS
////////////////////////////////////////////////////////////

#ifdef __APPLE__

#include <TargetConditionals.h>

#if ((TARGET_OS_IPHONE) || (TARGET_IPHONE_SIMULATOR))

#define PlatformiOS

#else

#define PlatformMac

#endif

#elif ((_WIN32) || (_WIN64))

#define DagonPlatformWindows

#elif __linux || defined(__FreeBSD__)

#define PlatformLinux

#endif

////////////////////////////////////////////////////////////
// Include standard OpenGL headers
////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include <GLFW/glfw3.h>

////////////////////////////////////////////////////////////
// Include standard C libraries
////////////////////////////////////////////////////////////

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

////////////////////////////////////////////////////////////
// Include standard C++ libraries
////////////////////////////////////////////////////////////

#include <algorithm>
#include <chrono>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

////////////////////////////////////////////////////////////
// Include common Dagon headers
////////////////////////////////////////////////////////////

#include "Colors.h"
#include "Defines.h"
#include "Geometry.h"
#include "Language.h"
#include "Object.h"
#include "Version.h"

////////////////////////////////////////////////////////////
// Key definitions
////////////////////////////////////////////////////////////

enum InputKeys {
	kKeyBackspace = GLFW_KEY_BACKSPACE,
  kKeyF1 = GLFW_KEY_F1,
	kKeyF2 = GLFW_KEY_F2,
	kKeyF3 = GLFW_KEY_F3,
	kKeyF4 = GLFW_KEY_F4,
  kKeyF5 = GLFW_KEY_F5,
	kKeyF6 = GLFW_KEY_F6,
	kKeyF7 = GLFW_KEY_F7,
	kKeyF8 = GLFW_KEY_F8,
	kKeyF9 = GLFW_KEY_F9,
	kKeyF10 = GLFW_KEY_F10,
	kKeyF11 = GLFW_KEY_F11,
	kKeyF12 = GLFW_KEY_F12,
	kKeyEscape = GLFW_KEY_ESCAPE,
	kKeyQuote = 0x60,
	kKeyTab = GLFW_KEY_TAB,
	kKeyEnter = GLFW_KEY_ENTER,
	kKeySpace = GLFW_KEY_SPACE
};

#endif // DAGON_PLATFORM_H_
