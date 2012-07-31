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
    
    if (strcmp(key, "fov") == 0) {
		lua_pushnumber(L, DGCameraManager::getInstance().fieldOfView());
		return 1;
	}
    
    if (strcmp(key, "horizontal") == 0) {
        lua_pushnumber(L, DGCameraManager::getInstance().angleHorizontal());
		return 1;
	}
    
    if (strcmp(key, "inertia") == 0) {
        lua_pushnumber(L, DGCameraManager::getInstance().inertia());
		return 1;
	}
    
    if (strcmp(key, "neutralZone") == 0) {
        lua_pushnumber(L, DGCameraManager::getInstance().neutralZone());
		return 1;
	}
    
    
    if (strcmp(key, "speed") == 0) {
        lua_pushnumber(L, DGCameraManager::getInstance().maxSpeed());
		return 1;
	}
    
    if (strcmp(key, "smooth") == 0) {
        lua_pushnumber(L, DGCameraManager::getInstance().speedFactor());
		return 1;
	}
    
    if (strcmp(key, "vertical") == 0) {
        lua_pushnumber(L, DGCameraManager::getInstance().angleVertical());
		return 1;
	}
    
    if (strcmp(key, "verticalLimit") == 0) {
        lua_pushnumber(L, DGCameraManager::getInstance().verticalLimit());
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
    
    if (strcmp(key, "fov") == 0)
		DGCameraManager::getInstance().setFieldOfView(lua_tonumber(L, 3));
    
    if (strcmp(key, "horizontal") == 0)
		DGCameraManager::getInstance().setAngleHorizontal(lua_tonumber(L, 3));
    
    if (strcmp(key, "inertia") == 0)
		DGCameraManager::getInstance().setInertia(lua_tonumber(L, 3));
    
    if (strcmp(key, "neutralZone") == 0)
		DGCameraManager::getInstance().setNeutralZone(lua_tonumber(L, 3));
    
    if (strcmp(key, "speed") == 0)
		DGCameraManager::getInstance().setMaxSpeed(lua_tonumber(L, 3));
    
    if (strcmp(key, "smooth") == 0)
		DGCameraManager::getInstance().setSpeedFactor(lua_tonumber(L, 3));
    
    if (strcmp(key, "vertical") == 0)
		DGCameraManager::getInstance().setAngleVertical(lua_tonumber(L, 3));
    
    if (strcmp(key, "verticalLimit") == 0)
		DGCameraManager::getInstance().setVerticalLimit(lua_tonumber(L, 3));
    
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
