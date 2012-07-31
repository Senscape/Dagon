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

#ifndef DG_CAMLIB_H
#define DG_CAMLIB_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGCameraManager.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

static int DGCamLibGet(lua_State *L) {
	const char *key = luaL_checkstring(L, 2);
	
	if (strcmp(key, "breathe") == 0) {
		lua_pushboolean(L, DGCameraManager::getInstance().canBreathe());
		return 1;
	}
    
	if (strcmp(key, "walk") == 0) {
		lua_pushboolean(L, DGCameraManager::getInstance().canWalk());
		return 1;
	}
	
	return 0;
}

static int DGCamLibSet(lua_State *L) {
	const char *key = luaL_checkstring(L, 2);
	
	if (strcmp(key, "breathe") == 0)
		DGCameraManager::getInstance().setBreathe((bool)lua_toboolean(L, 3));
    
	if (strcmp(key, "walk") == 0)
		DGCameraManager::getInstance().setWalk((bool)lua_toboolean(L, 3));
	
	return 0;
}

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const struct luaL_Reg DGCamLib[] =
{
    {"__index", DGCamLibGet},
    {"__newindex", DGCamLibSet},
    {NULL, NULL}
};

#endif // DG_CAMLIB_H
