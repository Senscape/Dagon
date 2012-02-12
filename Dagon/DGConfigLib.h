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

#ifndef DG_CONFIGLIB_H
#define DG_CONFIGLIB_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

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
    
    if (strcmp(key, "autorun") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().autorun);
		return 1;
	}
	
	if (strcmp(key, "display_width") == 0) {
		lua_pushnumber(L, DGConfig::getInstance().displayWidth);
		return 1;
	}
	
	if (strcmp(key, "display_height") == 0) {
		lua_pushnumber(L, DGConfig::getInstance().displayHeight);
		return 1;
	}
	
	if (strcmp(key, "display_depth") == 0) {
		lua_pushnumber(L, DGConfig::getInstance().displayDepth);
		return 1;
	}
	
	if (strcmp(key, "debug_mode") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().debugMode);
		return 1;
	}
	
	if (strcmp(key, "effects") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().effects);
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
	
	if (strcmp(key, "script") == 0) {
		lua_pushstring(L, DGConfig::getInstance().script());
		return 1;
	}
	
	if (strcmp(key, "show_splash") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().showSplash);
		return 1;
	}
	
	if (strcmp(key, "show_spots") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().showSpots);
		return 1;
	}
	
	if (strcmp(key, "tex_compression") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().texCompression);
		return 1;
	}
    
	if (strcmp(key, "tex_extension") == 0) {
		lua_pushstring(L, DGConfig::getInstance().texExtension());
		return 1;
	}

	if (strcmp(key, "vertical_sync") == 0) {
		lua_pushboolean(L, DGConfig::getInstance().verticalSync);
		return 1;
	}
	
	return 0;
}

static int DGConfigLibSet(lua_State *L) {
	const char *key = luaL_checkstring(L, 2);
	
	if (strcmp(key, "antialiasing") == 0)
		DGConfig::getInstance().antialiasing = lua_toboolean(L, 3);
    
    if (strcmp(key, "autorun") == 0)
		DGConfig::getInstance().antialiasing = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "display_width") == 0)
		DGConfig::getInstance().displayWidth = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "display_height") == 0)
		DGConfig::getInstance().displayHeight = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "display_depth") == 0)
		DGConfig::getInstance().displayDepth = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "debug_mode") == 0)
		DGConfig::getInstance().debugMode = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "effects") == 0)
		DGConfig::getInstance().effects = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "framerate") == 0)
		DGConfig::getInstance().framerate = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "fullscreen") == 0)
		DGConfig::getInstance().fullScreen = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "log") == 0)
		DGConfig::getInstance().log = (bool)lua_toboolean(L, 3);
    
    if (strcmp(key, "script") == 0)
        DGConfig::getInstance().setScript(luaL_checkstring(L, 3));
	
	if (strcmp(key, "show_splash") == 0)
		DGConfig::getInstance().showSplash = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "show_spots") == 0)
		DGConfig::getInstance().showSpots = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "tex_compression") == 0)
		DGConfig::getInstance().texCompression = (bool)lua_toboolean(L, 3);
	
	if (strcmp(key, "tex_extension") == 0)
        DGConfig::getInstance().setTexExtension(luaL_checkstring(L, 3));
    
	if (strcmp(key, "vertical_sync") == 0)
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
