/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_PLATFORM_H
#define	DG_PLATFORM_H

#ifdef __APPLE__

#include <TargetConditionals.h>

#if ((TARGET_OS_IPHONE) || (TARGET_IPHONE_SIMULATOR))

#define DG_PLATFORM_IOS
#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#else

#endif

#else

#define DG_PLATFORM_MAC
#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#endif

#endif

#elif ((_WIN32) || (_WIN64))

#define DG_PLATFORM_WIN
#include <windows.h>

#elif __linux

#define DG_PLATFORM_LINUX
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>

#endif


#ifdef DG_USE_SDL

#define	dg_main(x, y)	SDL_main(x, y)

#else

#define dg_main(x, y)	main(x, y)

#endif


// Our fancy BOOL type

typedef signed char DG_BOOL;

#define DG_YES  ((DG_BOOL)1)
#define DG_NO   ((DG_BOOL)0)


// Key definitions

enum DG_Keys {
#ifdef DG_PLATFORM_IOS
	DG_KEY_BACKSPACE = 0,
	DG_KEY_F1 = 0,
	DG_KEY_F2 = 0,
	DG_KEY_F3 = 0,	
	DG_KEY_F4 = 0,
	DG_KEY_F5 = 0,
	DG_KEY_F6 = 0,	
	DG_KEY_F7 = 0,	
	DG_KEY_F8 = 0,	
	DG_KEY_F9 = 0,
	DG_KEY_F10 = 0,
	DG_KEY_F11 = 0,
	DG_KEY_F12 = 0,
	DG_KEY_ESC = 0,
	DG_KEY_QUOTE = 0,
	DG_KEY_ENTER = 0
#endif
    
#ifdef DG_PLATFORM_MAC	
	DG_KEY_BACKSPACE = 0x7f,
	DG_KEY_F1 = 0x7a,
	DG_KEY_F2 = 0x78,
	DG_KEY_F3 = 0x63,	
	DG_KEY_F4 = 0x76,
	DG_KEY_F5 = 0x60,
	DG_KEY_F6 = 0x61,	
	DG_KEY_F7 = 0x62,	
	DG_KEY_F8 = 0x64,	
	DG_KEY_F9 = 0x65,
	DG_KEY_F10 = 0x6d,
	DG_KEY_F11 = 0x67,
	DG_KEY_F12 = 0x6f,
	DG_KEY_ESC = 0x1b,
	DG_KEY_QUOTE = 0x60,
	DG_KEY_ENTER = 0xd
#endif

#ifdef DG_PLATFORM_WIN
	DG_KEY_BACKSPACE = 8, // Look for hex value
	DG_KEY_F1 = VK_F1,
	DG_KEY_F2 = VK_F2,
	DG_KEY_F3 = VK_F3,	
	DG_KEY_F4 = VK_F4,
	DG_KEY_F5 = VK_F5,
	DG_KEY_F6 = VK_F6,	
	DG_KEY_F7 = VK_F7,	
	DG_KEY_F8 = VK_F8,	
	DG_KEY_F9 = VK_F9,
	DG_KEY_F10 = VK_F10,
	DG_KEY_F11 = VK_F11,
	DG_KEY_F12 = VK_F12,
	DG_KEY_ESC = 0x1b,
	DG_KEY_QUOTE = 0x60,
	DG_KEY_ENTER = 0xd
#endif
};

#endif // DG_PLATFORM_H
