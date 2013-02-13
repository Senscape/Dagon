////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2012 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
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
		lua_pushboolean(L, DGConfig::getInstance().antialiasing);
		return 1;
	}
    
    if (strcmp(key, "audioBuffer") == 0) {
		lua_pushnumber(L, DGConfig::getInstance().audioBuffer);
		return 1;
	}
    
    if (strcmp(key, "audioDevice") == 0) {
		lua_pushnumber(L, DGConfig::getInstance().audioDevice);
		return 1;
	}
    
    if (strcmp(key, "autopaths") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().autopaths);
		return 1;
	}
    
    if (strcmp(key, "autorun") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().autorun);
		return 1;
	}
    
    if (strcmp(key, "bundleEnabled") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().bundleEnabled);
		return 1;
	}
    
    if (strcmp(key, "controlMode") == 0) {
		lua_pushnumber(L, DGConfig::getInstance().controlMode);
		return 1;
	}
	
	if (strcmp(key, "displayWidth") == 0) {
		lua_pushnumber(L, DGConfig::getInstance().displayWidth);
		return 1;
	}
	
	if (strcmp(key, "displayHeight") == 0) {
		lua_pushnumber(L, DGConfig::getInstance().displayHeight);
		return 1;
	}
	
	if (strcmp(key, "displayDepth") == 0) {
		lua_pushnumber(L, DGConfig::getInstance().displayDepth);
		return 1;
	}
	
	if (strcmp(key, "debugMode") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().debugMode);
		return 1;
	}
	
	if (strcmp(key, "effects") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().effects);
		return 1;
	}

	if (strcmp(key, "forcedFullscreen") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().forcedFullScreen);
		return 1;
	}
    
    if (strcmp(key, "framebuffer") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().framebuffer);
		return 1;
	}
	
	if (strcmp(key, "framerate") == 0) {
		lua_pushnumber(L, DGConfig::getInstance().framerate);
		return 1;
	}
	
	if (strcmp(key, "fullscreen") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().fullScreen);
		return 1;
	}
	
	if (strcmp(key, "log") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().log);
		return 1;
	}
    
    if (strcmp(key, "mute") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().mute);
		return 1;
	}
	
	if (strcmp(key, "script") == 0) {
		lua_pushstring(L, DGConfig::getInstance().script());
		return 1;
	}
    
    if (strcmp(key, "showHelpers") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().showHelpers);
		return 1;
	}
	
	if (strcmp(key, "showSplash") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().showSplash);
		return 1;
	}
	
	if (strcmp(key, "showSpots") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().showSpots);
		return 1;
	}
    
	if (strcmp(key, "subtitles") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().subtitles);
		return 1;
	}
    
	if (strcmp(key, "silentFeeds") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().silentFeeds);
		return 1;
	}
	
	if (strcmp(key, "texCompression") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().texCompression);
		return 1;
	}
    
	if (strcmp(key, "texExtension") == 0) {
		lua_pushstring(L, DGConfig::getInstance().texExtension());
		return 1;
	}

	if (strcmp(key, "verticalSync") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().verticalSync);
		return 1;
	}
	
	return 0;
}

static int DGConfigLibSet(lua_State *L) {
	const char *key = luaL_checkstring(L, 2);
	
	if (strcmp(key, "antialiasing") == 0)
		DGConfig::getInstance().antialiasing = (bool)lua_toboolean(L, 3);

    if (strcmp(key, "audioBuffer") == 0)
		DGConfig::getInstance().audioBuffer = (int)luaL_checknumber(L, 3);

    if (strcmp(key, "audioDevice") == 0)
		DGConfig::getInstance().audioDevice = (int)luaL_checknumber(L, 3);
    
	if (strcmp(key, "autopaths") == 0)
		DGConfig::getInstance().autopaths = (bool)lua_toboolean(L, 3);    
    
    if (strcmp(key, "autorun") == 0)
		DGConfig::getInstance().autorun = (bool)lua_toboolean(L, 3);
    
    if (strcmp(key, "bundleEnabled") == 0)
		DGConfig::getInstance().bundleEnabled = (bool)lua_toboolean(L, 3);    
    
    if (strcmp(key, "controlMode") == 0) {
		DGConfig::getInstance().controlMode = (int)luaL_checknumber(L, 3);
        // Must refresh the viewport with this change
        DGCameraManager::getInstance().setViewport(DGConfig::getInstance().displayWidth, DGConfig::getInstance().displayHeight);
    }
	
	if (strcmp(key, "displayWidth") == 0)
		DGConfig::getInstance().displayWidth = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "displayHeight") == 0)
		DGConfig::getInstance().displayHeight = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "displayDepth") == 0)
		DGConfig::getInstance().displayDepth = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "debugMode") == 0)
		DGConfig::getInstance().debugMode = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "effects") == 0)
		DGConfig::getInstance().effects = (bool)lua_toboolean(L, 3);

	if (strcmp(key, "forcedFullscreen") == 0)
		DGConfig::getInstance().forcedFullScreen = (bool)lua_toboolean(L, 3);
    
	if (strcmp(key, "framebuffer") == 0)
		DGConfig::getInstance().framebuffer = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "framerate") == 0)
		DGConfig::getInstance().framerate = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "fullscreen") == 0)
		DGConfig::getInstance().fullScreen = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "log") == 0)
		DGConfig::getInstance().log = (bool)lua_toboolean(L, 3);
    
    if (strcmp(key, "mute") == 0)
		DGConfig::getInstance().mute = (bool)lua_toboolean(L, 3);
    
    if (strcmp(key, "script") == 0)
        DGConfig::getInstance().setScript(luaL_checkstring(L, 3));
    
	if (strcmp(key, "showHelpers") == 0)
		DGConfig::getInstance().showHelpers = (bool)lua_toboolean(L, 3);    
	
	if (strcmp(key, "showSplash") == 0)
		DGConfig::getInstance().showSplash = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "showSpots") == 0)
		DGConfig::getInstance().showSpots = (bool)lua_toboolean(L, 3);
    
	if (strcmp(key, "silentFeeds") == 0)
		DGConfig::getInstance().silentFeeds = (bool)lua_toboolean(L, 3);

    if (strcmp(key, "subtitles") == 0)
		DGConfig::getInstance().subtitles = (bool)lua_toboolean(L, 3);
    
	if (strcmp(key, "texCompression") == 0)
		DGConfig::getInstance().texCompression = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "texExtension") == 0)
        DGConfig::getInstance().setTexExtension(luaL_checkstring(L, 3));
    
	if (strcmp(key, "verticalSync") == 0)
		DGConfig::getInstance().verticalSync = (bool)lua_toboolean(L, 3);
	
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
