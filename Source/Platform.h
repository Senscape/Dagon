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
#include <SDL2/SDL_keycode.h>

////////////////////////////////////////////////////////////
// Include standard C libraries
////////////////////////////////////////////////////////////

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

////////////////////////////////////////////////////////////
// Include standard C++ libraries
////////////////////////////////////////////////////////////

#include <algorithm>
#include <fstream>
#include <string>
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
  kKeyBackspace = SDLK_BACKSPACE,
  kKeyF1 = SDLK_F1,
  kKeyF2 = SDLK_F2,
  kKeyF3 = SDLK_F3,
  kKeyF4 = SDLK_F4,
  kKeyF5 = SDLK_F5,
  kKeyF6 = SDLK_F6,
  kKeyF7 = SDLK_F7,
  kKeyF8 = SDLK_F8,
  kKeyF9 = SDLK_F9,
  kKeyF10 = SDLK_F10,
  kKeyF11 = SDLK_F11,
  kKeyF12 = SDLK_F12,
  kKeyEscape = SDLK_ESCAPE,
  kKeyQuote = SDL_SCANCODE_GRAVE,
  kKeyTab = SDLK_TAB,
  kKeyEnter = SDLK_RETURN,
  kKeySpace = SDLK_SPACE
};

#endif // DAGON_PLATFORM_H_
