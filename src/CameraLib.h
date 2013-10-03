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

#ifndef DAGON_CAMERALIB_H_
#define DAGON_CAMERALIB_H_

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by Proxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "CameraManager.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

static int CameraLibGet(lua_State *L) {
  const char *key = luaL_checkstring(L, 2);
  
  if (strcmp(key, "breathe") == 0) {
    lua_pushboolean(L, CameraManager::instance().canBreathe());
    return 1;
  }
  
  if (strcmp(key, "fov") == 0) {
    lua_pushnumber(L, CameraManager::instance().fieldOfView());
    return 1;
  }
  
  if (strcmp(key, "horizontal") == 0) {
    lua_pushnumber(L, CameraManager::instance().angleHorizontal());
    return 1;
  }
  
  if (strcmp(key, "inertia") == 0) {
    lua_pushnumber(L, CameraManager::instance().inertia());
    return 1;
  }
  
  if (strcmp(key, "neutralZone") == 0) {
    lua_pushnumber(L, CameraManager::instance().neutralZone());
    return 1;
  }
  
  
  if (strcmp(key, "speed") == 0) {
    lua_pushnumber(L, CameraManager::instance().maxSpeed());
    return 1;
  }
  
  if (strcmp(key, "smooth") == 0) {
    lua_pushnumber(L, CameraManager::instance().speedFactor());
    return 1;
  }
  
  if (strcmp(key, "vertical") == 0) {
    lua_pushnumber(L, CameraManager::instance().angleVertical());
    return 1;
  }
  
  if (strcmp(key, "verticalLimit") == 0) {
    lua_pushnumber(L, CameraManager::instance().verticalLimit());
    return 1;
  }
  
  if (strcmp(key, "walk") == 0) {
    lua_pushboolean(L, CameraManager::instance().canWalk());
    return 1;
  }
  
  return 0;
}

static int CameraLibSet(lua_State *L) {
  const char *key = luaL_checkstring(L, 2);
  
  if (strcmp(key, "breathe") == 0)
    CameraManager::instance().setBreathe((bool)lua_toboolean(L, 3));
  if (strcmp(key, "fov") == 0)
    CameraManager::instance().setFieldOfView(static_cast<float>(lua_tonumber(L, 3)));
  
  if (strcmp(key, "horizontal") == 0)
    CameraManager::instance().setAngleHorizontal(static_cast<float>(lua_tonumber(L, 3)));
  
  if (strcmp(key, "inertia") == 0)
    CameraManager::instance().setInertia(static_cast<int>(lua_tonumber(L, 3)));
  
  if (strcmp(key, "neutralZone") == 0)
    CameraManager::instance().setNeutralZone(static_cast<int>(lua_tonumber(L, 3)));
  
  if (strcmp(key, "speed") == 0)
    CameraManager::instance().setMaxSpeed(static_cast<int>(lua_tonumber(L, 3)));
  
  if (strcmp(key, "smooth") == 0)
    CameraManager::instance().setSpeedFactor(static_cast<int>(lua_tonumber(L, 3)));
  
  if (strcmp(key, "vertical") == 0)
    CameraManager::instance().setAngleVertical(static_cast<float>(lua_tonumber(L, 3)));
  
  if (strcmp(key, "verticalLimit") == 0)
    CameraManager::instance().setVerticalLimit(static_cast<float>(lua_tonumber(L, 3)));
  
  if (strcmp(key, "walk") == 0)
    CameraManager::instance().setWalk((bool)lua_toboolean(L, 3));
  
  return 0;
}

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const struct luaL_Reg CameraLib[] =
{
  {"__index", CameraLibGet},
  {"__newindex", CameraLibSet},
  {NULL, NULL}
};
  
}

#endif // DAGON_CAMERALIB_H_
