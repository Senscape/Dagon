////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
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
#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#else

#endif

#else

#define DGPlatformMac
#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#endif

#endif

#elif ((_WIN32) || (_WIN64))

#define DGPlatformWindows
#include <Windows.h>

// We use this function a lot but its standard isn't supported
// by MSVC (go figure). While this isn't entirely correct, it
// does the trick.
#if _MSC_VER
#define snprintf _snprintf
#endif

#elif __linux

#define DGPlatformLinux
#include <X11/Xlib.h>              
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/extensions/xf86vmode.h>

#endif

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
#include <fstream>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////
// Include common Dagon headers
////////////////////////////////////////////////////////////

#include "DGColors.h"
#include "DGDefines.h"
#include "DGGeometry.h"
#include "DGLanguage.h"
// This is such a common module that we're including it here too
#include "DGObject.h"
#include "DGVersion.h"

////////////////////////////////////////////////////////////
// Key definitions
////////////////////////////////////////////////////////////

#ifdef DGPlatformLinux
enum DGKeys {
	DGKeyBackspace = XK_BackSpace,
    DGKeyF1 = XK_F1,
	DGKeyF2 = XK_F2,
	DGKeyF3 = XK_F3,	
	DGKeyF4 = XK_F4,
    DGKeyF5 = XK_F5,
	DGKeyF6 = XK_F6,	
	DGKeyF7 = XK_F7,	
	DGKeyF8 = XK_F8,	
	DGKeyF9 = XK_F9,
	DGKeyF10 = XK_F10,
	DGKeyF11 = XK_F11,
	DGKeyF12 = XK_F12,
	DGKeyEsc = XK_Escape,
	DGKeyQuote = 0x60,
	DGKeyTab = XK_Tab,    
	DGKeyEnter = XK_Return,
	DGKeySpacebar = 0x20    
};
#endif

#ifdef DGPlatformMac
enum DGKeys {
    // These are all Mac key codes
    DGKeyBackspace = 0x7f,
    DGKeyF1 = 0x7a,
	DGKeyF2 = 0x78,
	DGKeyF3 = 0x63,	
	DGKeyF4 = 0x76,
    DGKeyF5 = 0x60,
	DGKeyF6 = 0x61,	
	DGKeyF7 = 0x62,	
	DGKeyF8 = 0x64,	
	DGKeyF9 = 0x65,
	DGKeyF10 = 0x6d,
	DGKeyF11 = 0x67,
	DGKeyF12 = 0x6f,
	DGKeyEsc = 0x1b,
	DGKeyQuote = 0x60,
	DGKeyTab = 0x9,    
	DGKeyEnter = 0xd,
	DGKeySpacebar = 0x20
};
#endif
    
#ifdef DGPlatformWindows
enum DGKeys {
    // This is a mix of Windows defs and unicode chars
	DGKeyBackspace = 8, // Look for hex value
	DGKeyF1 = VK_F1,
	DGKeyF2 = VK_F2,
	DGKeyF3 = VK_F3,	
	DGKeyF4 = VK_F4,
	DGKeyF5 = VK_F5,
	DGKeyF6 = VK_F6,	
	DGKeyF7 = VK_F7,	
	DGKeyF8 = VK_F8,	
	DGKeyF9 = VK_F9,
	DGKeyF10 = VK_F10,
	DGKeyF11 = VK_F11,
	DGKeyF12 = VK_F12,
	DGKeyEsc = 0x1b,
	DGKeyQuote = 0x60,
	DGKeyTab = 0x9,    
	DGKeyEnter = 0xd,
	DGKeySpacebar = 0x20    
};
#endif

#endif // DG_PLATFORM_H
