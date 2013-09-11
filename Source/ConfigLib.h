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
#include "Config.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

// TODO: Support changing the paths

static int ConfigLibGet(lua_State *L) {
	const char *key = luaL_checkstring(L, 2);
	
	if (strcmp(key, "antialiasing") == 0) {
		lua_pushboolean(L, Config::instance().antialiasing);
		return 1;
	}
    
    if (strcmp(key, "audioBuffer") == 0) {
		lua_pushnumber(L, Config::instance().audioBuffer);
		return 1;
	}
    
    if (strcmp(key, "audioDevice") == 0) {
		lua_pushnumber(L, Config::instance().audioDevice);
		return 1;
	}
    
    if (strcmp(key, "autopaths") == 0) {
		lua_pushboolean(L, Config::instance().autopaths);
		return 1;
	}
    
    if (strcmp(key, "autorun") == 0) {
		lua_pushboolean(L, Config::instance().autorun);
		return 1;
	}
    
    if (strcmp(key, "bundleEnabled") == 0) {
		lua_pushboolean(L, Config::instance().bundleEnabled);
		return 1;
	}
    
    if (strcmp(key, "controlMode") == 0) {
		lua_pushnumber(L, Config::instance().controlMode);
		return 1;
	}
	
	if (strcmp(key, "displayWidth") == 0) {
		lua_pushnumber(L, Config::instance().displayWidth);
		return 1;
	}
	
	if (strcmp(key, "displayHeight") == 0) {
		lua_pushnumber(L, Config::instance().displayHeight);
		return 1;
	}
	
	if (strcmp(key, "displayDepth") == 0) {
		lua_pushnumber(L, Config::instance().displayDepth);
		return 1;
	}
	
	if (strcmp(key, "debugMode") == 0) {
		lua_pushboolean(L, Config::instance().debugMode);
		return 1;
	}
	
	if (strcmp(key, "effects") == 0) {
		lua_pushboolean(L, Config::instance().effects);
		return 1;
	}
    
    if (strcmp(key, "framebuffer") == 0) {
		lua_pushboolean(L, Config::instance().framebuffer);
		return 1;
	}
	
	if (strcmp(key, "framerate") == 0) {
		lua_pushnumber(L, Config::instance().framerate);
		return 1;
	}
	
	if (strcmp(key, "fullscreen") == 0) {
		lua_pushboolean(L, Config::instance().fullscreen);
		return 1;
	}
	
	if (strcmp(key, "log") == 0) {
		lua_pushboolean(L, Config::instance().log);
		return 1;
	}
    
    if (strcmp(key, "mute") == 0) {
		lua_pushboolean(L, Config::instance().mute);
		return 1;
	}
	
	if (strcmp(key, "script") == 0) {
		lua_pushstring(L, Config::instance().script().c_str());
		return 1;
	}
    
    if (strcmp(key, "showHelpers") == 0) {
		lua_pushboolean(L, Config::instance().showHelpers);
		return 1;
	}
	
	if (strcmp(key, "showSplash") == 0) {
		lua_pushboolean(L, Config::instance().showSplash);
		return 1;
	}
	
	if (strcmp(key, "showSpots") == 0) {
		lua_pushboolean(L, Config::instance().showSpots);
		return 1;
	}
    
	if (strcmp(key, "subtitles") == 0) {
		lua_pushboolean(L, Config::instance().subtitles);
		return 1;
	}
    
	if (strcmp(key, "silentFeeds") == 0) {
		lua_pushboolean(L, Config::instance().silentFeeds);
		return 1;
	}
	
	if (strcmp(key, "texCompression") == 0) {
		lua_pushboolean(L, Config::instance().texCompression);
		return 1;
	}
    
	if (strcmp(key, "texExtension") == 0) {
		lua_pushstring(L, Config::instance().texExtension().c_str());
		return 1;
	}

	if (strcmp(key, "verticalSync") == 0) {
		lua_pushboolean(L, Config::instance().verticalSync);
		return 1;
	}
	
	return 0;
}

static int ConfigLibSet(lua_State *L) {
	const char *key = luaL_checkstring(L, 2);
	
	if (strcmp(key, "antialiasing") == 0)
		Config::instance().antialiasing = (bool)lua_toboolean(L, 3);

    if (strcmp(key, "audioBuffer") == 0)
		Config::instance().audioBuffer = (int)luaL_checknumber(L, 3);

    if (strcmp(key, "audioDevice") == 0)
		Config::instance().audioDevice = (int)luaL_checknumber(L, 3);
    
	if (strcmp(key, "autopaths") == 0)
		Config::instance().autopaths = (bool)lua_toboolean(L, 3);    
    
    if (strcmp(key, "autorun") == 0)
		Config::instance().autorun = (bool)lua_toboolean(L, 3);
    
    if (strcmp(key, "bundleEnabled") == 0)
		Config::instance().bundleEnabled = (bool)lua_toboolean(L, 3);    
    
    if (strcmp(key, "controlMode") == 0) {
		Config::instance().controlMode = (int)luaL_checknumber(L, 3);
        // Must refresh the viewport with this change
        DGCameraManager::instance().setViewport(Config::instance().displayWidth, Config::instance().displayHeight);
    }
	
	if (strcmp(key, "displayWidth") == 0)
		Config::instance().displayWidth = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "displayHeight") == 0)
		Config::instance().displayHeight = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "displayDepth") == 0)
		Config::instance().displayDepth = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "debugMode") == 0)
		Config::instance().debugMode = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "effects") == 0)
		Config::instance().effects = (bool)lua_toboolean(L, 3);
    
	if (strcmp(key, "framebuffer") == 0)
		Config::instance().framebuffer = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "framerate") == 0)
		Config::instance().framerate = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "fullscreen") == 0)
		Config::instance().fullscreen = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "log") == 0)
		Config::instance().log = (bool)lua_toboolean(L, 3);
    
    if (strcmp(key, "mute") == 0)
		Config::instance().mute = (bool)lua_toboolean(L, 3);
    
    if (strcmp(key, "script") == 0)
        Config::instance().setScript(luaL_checkstring(L, 3));
    
	if (strcmp(key, "showHelpers") == 0)
		Config::instance().showHelpers = (bool)lua_toboolean(L, 3);    
	
	if (strcmp(key, "showSplash") == 0)
		Config::instance().showSplash = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "showSpots") == 0)
		Config::instance().showSpots = (bool)lua_toboolean(L, 3);
    
	if (strcmp(key, "silentFeeds") == 0)
		Config::instance().silentFeeds = (bool)lua_toboolean(L, 3);

    if (strcmp(key, "subtitles") == 0)
		Config::instance().subtitles = (bool)lua_toboolean(L, 3);
    
	if (strcmp(key, "texCompression") == 0)
		Config::instance().texCompression = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "texExtension") == 0)
        Config::instance().setTexExtension(luaL_checkstring(L, 3));
    
	if (strcmp(key, "verticalSync") == 0)
		Config::instance().verticalSync = (bool)lua_toboolean(L, 3);
	
	return 0;
}

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const struct luaL_Reg ConfigLib[] =
{
    {"__index", ConfigLibGet},
    {"__newindex", ConfigLibSet},
    {NULL, NULL}
};

#endif // DG_CONFIGLIB_H
