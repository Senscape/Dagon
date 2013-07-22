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

#ifndef DG_CONFIGLIB_H
#define DG_CONFIGLIB_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGCameraManager.h"
#include "DGConfig.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

// TODO: Support changing the paths

static int DGConfigLibGet(lua_State *L) {
	const char *key = luaL_checkstring(L, 2);
	
	if (strcmp(key, "antialiasing") == 0) {
		lua_pushboolean(L, DGConfig::instance().antialiasing);
		return 1;
	}
    
    if (strcmp(key, "audioBuffer") == 0) {
		lua_pushnumber(L, DGConfig::instance().audioBuffer);
		return 1;
	}
    
    if (strcmp(key, "audioDevice") == 0) {
		lua_pushnumber(L, DGConfig::instance().audioDevice);
		return 1;
	}
    
    if (strcmp(key, "autopaths") == 0) {
		lua_pushboolean(L, DGConfig::instance().autopaths);
		return 1;
	}
    
    if (strcmp(key, "autorun") == 0) {
		lua_pushboolean(L, DGConfig::instance().autorun);
		return 1;
	}
    
    if (strcmp(key, "bundleEnabled") == 0) {
		lua_pushboolean(L, DGConfig::instance().bundleEnabled);
		return 1;
	}
    
    if (strcmp(key, "controlMode") == 0) {
		lua_pushnumber(L, DGConfig::instance().controlMode);
		return 1;
	}
	
	if (strcmp(key, "displayWidth") == 0) {
		lua_pushnumber(L, DGConfig::instance().displayWidth);
		return 1;
	}
	
	if (strcmp(key, "displayHeight") == 0) {
		lua_pushnumber(L, DGConfig::instance().displayHeight);
		return 1;
	}
	
	if (strcmp(key, "displayDepth") == 0) {
		lua_pushnumber(L, DGConfig::instance().displayDepth);
		return 1;
	}
	
	if (strcmp(key, "debugMode") == 0) {
		lua_pushboolean(L, DGConfig::instance().debugMode);
		return 1;
	}
	
	if (strcmp(key, "effects") == 0) {
		lua_pushboolean(L, DGConfig::instance().effects);
		return 1;
	}
    
    if (strcmp(key, "framebuffer") == 0) {
		lua_pushboolean(L, DGConfig::instance().framebuffer);
		return 1;
	}
	
	if (strcmp(key, "framerate") == 0) {
		lua_pushnumber(L, DGConfig::instance().framerate);
		return 1;
	}
	
	if (strcmp(key, "fullscreen") == 0) {
		lua_pushboolean(L, DGConfig::instance().fullScreen);
		return 1;
	}
	
	if (strcmp(key, "log") == 0) {
		lua_pushboolean(L, DGConfig::instance().log);
		return 1;
	}
    
    if (strcmp(key, "mute") == 0) {
		lua_pushboolean(L, DGConfig::instance().mute);
		return 1;
	}
	
	if (strcmp(key, "script") == 0) {
		lua_pushstring(L, DGConfig::instance().script());
		return 1;
	}
    
    if (strcmp(key, "showHelpers") == 0) {
		lua_pushboolean(L, DGConfig::instance().showHelpers);
		return 1;
	}
	
	if (strcmp(key, "showSplash") == 0) {
		lua_pushboolean(L, DGConfig::instance().showSplash);
		return 1;
	}
	
	if (strcmp(key, "showSpots") == 0) {
		lua_pushboolean(L, DGConfig::instance().showSpots);
		return 1;
	}
    
	if (strcmp(key, "subtitles") == 0) {
		lua_pushboolean(L, DGConfig::instance().subtitles);
		return 1;
	}
    
	if (strcmp(key, "silentFeeds") == 0) {
		lua_pushboolean(L, DGConfig::instance().silentFeeds);
		return 1;
	}
	
	if (strcmp(key, "texCompression") == 0) {
		lua_pushboolean(L, DGConfig::instance().texCompression);
		return 1;
	}
    
	if (strcmp(key, "texExtension") == 0) {
		lua_pushstring(L, DGConfig::instance().texExtension());
		return 1;
	}

	if (strcmp(key, "verticalSync") == 0) {
		lua_pushboolean(L, DGConfig::instance().verticalSync);
		return 1;
	}
	
	return 0;
}

static int DGConfigLibSet(lua_State *L) {
	const char *key = luaL_checkstring(L, 2);
	
	if (strcmp(key, "antialiasing") == 0)
		DGConfig::instance().antialiasing = (bool)lua_toboolean(L, 3);

    if (strcmp(key, "audioBuffer") == 0)
		DGConfig::instance().audioBuffer = (int)luaL_checknumber(L, 3);

    if (strcmp(key, "audioDevice") == 0)
		DGConfig::instance().audioDevice = (int)luaL_checknumber(L, 3);
    
	if (strcmp(key, "autopaths") == 0)
		DGConfig::instance().autopaths = (bool)lua_toboolean(L, 3);    
    
    if (strcmp(key, "autorun") == 0)
		DGConfig::instance().autorun = (bool)lua_toboolean(L, 3);
    
    if (strcmp(key, "bundleEnabled") == 0)
		DGConfig::instance().bundleEnabled = (bool)lua_toboolean(L, 3);    
    
    if (strcmp(key, "controlMode") == 0) {
		DGConfig::instance().controlMode = (int)luaL_checknumber(L, 3);
        // Must refresh the viewport with this change
        DGCameraManager::instance().setViewport(DGConfig::instance().displayWidth, DGConfig::instance().displayHeight);
    }
	
	if (strcmp(key, "displayWidth") == 0)
		DGConfig::instance().displayWidth = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "displayHeight") == 0)
		DGConfig::instance().displayHeight = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "displayDepth") == 0)
		DGConfig::instance().displayDepth = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "debugMode") == 0)
		DGConfig::instance().debugMode = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "effects") == 0)
		DGConfig::instance().effects = (bool)lua_toboolean(L, 3);
    
	if (strcmp(key, "framebuffer") == 0)
		DGConfig::instance().framebuffer = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "framerate") == 0)
		DGConfig::instance().framerate = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "fullscreen") == 0)
		DGConfig::instance().fullScreen = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "log") == 0)
		DGConfig::instance().log = (bool)lua_toboolean(L, 3);
    
    if (strcmp(key, "mute") == 0)
		DGConfig::instance().mute = (bool)lua_toboolean(L, 3);
    
    if (strcmp(key, "script") == 0)
        DGConfig::instance().setScript(luaL_checkstring(L, 3));
    
	if (strcmp(key, "showHelpers") == 0)
		DGConfig::instance().showHelpers = (bool)lua_toboolean(L, 3);    
	
	if (strcmp(key, "showSplash") == 0)
		DGConfig::instance().showSplash = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "showSpots") == 0)
		DGConfig::instance().showSpots = (bool)lua_toboolean(L, 3);
    
	if (strcmp(key, "silentFeeds") == 0)
		DGConfig::instance().silentFeeds = (bool)lua_toboolean(L, 3);

    if (strcmp(key, "subtitles") == 0)
		DGConfig::instance().subtitles = (bool)lua_toboolean(L, 3);
    
	if (strcmp(key, "texCompression") == 0)
		DGConfig::instance().texCompression = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "texExtension") == 0)
        DGConfig::instance().setTexExtension(luaL_checkstring(L, 3));
    
	if (strcmp(key, "verticalSync") == 0)
		DGConfig::instance().verticalSync = (bool)lua_toboolean(L, 3);
	
	return 0;
}

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const struct luaL_Reg DGConfigLib[] =
{
    {"__index", DGConfigLibGet},
    {"__newindex", DGConfigLibSet},
    {NULL, NULL}
};

#endif // DG_CONFIGLIB_H
