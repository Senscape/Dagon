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

#ifndef DG_PLATFORM_H
#define DG_PLATFORM_H

////////////////////////////////////////////////////////////
// Detect the current OS
////////////////////////////////////////////////////////////

#ifdef __APPLE__

#include <TargetConditionals.h>

#if ((TARGET_OS_IPHONE) || (TARGET_IPHONE_SIMULATOR))

#define DGPlatformiOS

#else

#define DGPlatformMac

#endif

#elif ((_WIN32) || (_WIN64))

#define DGPlatformWindows

#elif __linux || defined(__FreeBSD__)

#define DGPlatformLinux

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
#include <mutex>
#include <string>
#include <thread>
#include <vector>

////////////////////////////////////////////////////////////
// Include common Dagon headers
////////////////////////////////////////////////////////////

#include "DGColors.h"
#include "Defines.h"
#include "DGGeometry.h"
#include "DGLanguage.h"
#include "DGObject.h"
#include "DGVersion.h"

////////////////////////////////////////////////////////////
// Key definitions
////////////////////////////////////////////////////////////

enum DGKeys {
	DGKeyBackspace = GLFW_KEY_BACKSPACE,
    DGKeyF1 = GLFW_KEY_F1,
	DGKeyF2 = GLFW_KEY_F2,
	DGKeyF3 = GLFW_KEY_F3,
	DGKeyF4 = GLFW_KEY_F4,
    DGKeyF5 = GLFW_KEY_F5,
	DGKeyF6 = GLFW_KEY_F6,
	DGKeyF7 = GLFW_KEY_F7,
	DGKeyF8 = GLFW_KEY_F8,
	DGKeyF9 = GLFW_KEY_F9,
	DGKeyF10 = GLFW_KEY_F10,
	DGKeyF11 = GLFW_KEY_F11,
	DGKeyF12 = GLFW_KEY_F12,
	DGKeyEsc = GLFW_KEY_ESCAPE,
	DGKeyQuote = 0x60,
	DGKeyTab = GLFW_KEY_TAB,
	DGKeyEnter = GLFW_KEY_ENTER,
	DGKeySpacebar = GLFW_KEY_SPACE
};

#endif // DG_PLATFORM_H
