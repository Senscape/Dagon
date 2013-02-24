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

#ifndef DG_CURSORLIB_H
#define DG_CURSORLIB_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGCursorManager.h"

// FIXME: The cursor manager shouldn't inherit from DGObject...
// In fact, it shouldn't be a singleton.

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

static int DGCursorLibDisable(lua_State *L) {
    DGCursorManager::getInstance().DGObject::disable();
	
	return 0;
}

static int DGCursorLibEnable(lua_State *L) {
    DGCursorManager::getInstance().DGObject::enable();
	
	return 0;
}

static int DGCursorLibFadeIn(lua_State *L) {
    DGCursorManager::getInstance().DGObject::fadeIn();
	
	return 0;
}

static int DGCursorLibFadeOut(lua_State *L) {
    DGCursorManager::getInstance().DGObject::fadeOut();
	
	return 0;
}

static int DGCursorLibLoad(lua_State *L) {
    DGCursorManager::getInstance().load(luaL_checknumber(L, 1), luaL_checkstring(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    
    return 0;
}

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

static const struct luaL_reg DGCursorLib [] = {
	{"disable", DGCursorLibDisable},
	{"enable", DGCursorLibEnable},   
    {"fadeIn", DGCursorLibFadeIn}, 
    {"fadeOut", DGCursorLibFadeOut},
    {"load", DGCursorLibLoad},
	{NULL, NULL}
};

#endif // DG_CURSORLIB_H
