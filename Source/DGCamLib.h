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
        lua_pushboolean(L, DGCameraManager::instance().canBreathe());
        return 1;
    }
    
    if (strcmp(key, "fov") == 0) {
        lua_pushnumber(L, DGCameraManager::instance().fieldOfView());
        return 1;
    }
    
    if (strcmp(key, "horizontal") == 0) {
        lua_pushnumber(L, DGCameraManager::instance().angleHorizontal());
        return 1;
    }
    
    if (strcmp(key, "inertia") == 0) {
        lua_pushnumber(L, DGCameraManager::instance().inertia());
        return 1;
    }
    
    if (strcmp(key, "neutralZone") == 0) {
        lua_pushnumber(L, DGCameraManager::instance().neutralZone());
        return 1;
    }
    
    
    if (strcmp(key, "speed") == 0) {
        lua_pushnumber(L, DGCameraManager::instance().maxSpeed());
        return 1;
    }
    
    if (strcmp(key, "smooth") == 0) {
        lua_pushnumber(L, DGCameraManager::instance().speedFactor());
        return 1;
    }
    
    if (strcmp(key, "vertical") == 0) {
        lua_pushnumber(L, DGCameraManager::instance().angleVertical());
        return 1;
    }
    
    if (strcmp(key, "verticalLimit") == 0) {
        lua_pushnumber(L, DGCameraManager::instance().verticalLimit());
        return 1;
    }
    
    if (strcmp(key, "walk") == 0) {
        lua_pushboolean(L, DGCameraManager::instance().canWalk());
        return 1;
    }
    
    return 0;
}

static int DGCamLibSet(lua_State *L) {
    const char *key = luaL_checkstring(L, 2);
    
    if (strcmp(key, "breathe") == 0)
        DGCameraManager::instance().setBreathe((bool)lua_toboolean(L, 3));
    if (strcmp(key, "fov") == 0)
      DGCameraManager::instance().setFieldOfView(static_cast<float>(lua_tonumber(L, 3)));
    
    if (strcmp(key, "horizontal") == 0)
        DGCameraManager::instance().setAngleHorizontal(static_cast<float>(lua_tonumber(L, 3)));
    
    if (strcmp(key, "inertia") == 0)
        DGCameraManager::instance().setInertia(static_cast<int>(lua_tonumber(L, 3)));
    
    if (strcmp(key, "neutralZone") == 0)
        DGCameraManager::instance().setNeutralZone(static_cast<int>(lua_tonumber(L, 3)));
    
    if (strcmp(key, "speed") == 0)
        DGCameraManager::instance().setMaxSpeed(static_cast<int>(lua_tonumber(L, 3)));
    
    if (strcmp(key, "smooth") == 0)
        DGCameraManager::instance().setSpeedFactor(static_cast<int>(lua_tonumber(L, 3)));
    
    if (strcmp(key, "vertical") == 0)
        DGCameraManager::instance().setAngleVertical(static_cast<float>(lua_tonumber(L, 3)));
    
    if (strcmp(key, "verticalLimit") == 0)
        DGCameraManager::instance().setVerticalLimit(static_cast<float>(lua_tonumber(L, 3)));
    
    if (strcmp(key, "walk") == 0)
        DGCameraManager::instance().setWalk((bool)lua_toboolean(L, 3));
    
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
