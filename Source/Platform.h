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

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

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
	kKeyBackspace = sf::Keyboard::BackSpace,
  kKeyF1 = sf::Keyboard::F1,
	kKeyF2 = sf::Keyboard::F2,
	kKeyF3 = sf::Keyboard::F3,
	kKeyF4 = sf::Keyboard::F4,
  kKeyF5 = sf::Keyboard::F5,
	kKeyF6 = sf::Keyboard::F6,
	kKeyF7 = sf::Keyboard::F7,
	kKeyF8 = sf::Keyboard::F8,
	kKeyF9 = sf::Keyboard::F9,
	kKeyF10 = sf::Keyboard::F10,
	kKeyF11 = sf::Keyboard::F11,
	kKeyF12 = sf::Keyboard::F12,
	kKeyEscape = sf::Keyboard::Escape,
	kKeyQuote = sf::Keyboard::Quote,
	kKeyTab = sf::Keyboard::Tab,
	kKeyEnter = sf::Keyboard::Return,
	kKeySpace = sf::Keyboard::Space
};

#endif // DAGON_PLATFORM_H_
