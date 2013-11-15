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

#ifndef DAGON_EFFECTSLIB_H_
#define DAGON_EFFECTSLIB_H_

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by Proxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <stdint.h>

#include "EffectsManager.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

static int EffectsLibGet(lua_State *L) {
  EffectsManager& effectsManager = EffectsManager::instance();
  const char *key = luaL_checkstring(L, 2);
  
  // TODO: Check if valid first
  lua_pushnumber(L, effectsManager[key]);
  
  return 1;
}

static int EffectsLibSet(lua_State *L) {
  EffectsManager& effectsManager = EffectsManager::instance();
  const char *key = luaL_checkstring(L, 2);
  
  if (strcmp(key, "dustColor") == 0) {
    effectsManager.set(key, static_cast<uint32_t>(lua_tonumber(L, 3)));
  }
  else {
    effectsManager.set(key, static_cast<int>(lua_tonumber(L, 3)));
  }
  
  return 0;
}

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const struct luaL_Reg EffectsLib[] =
{
  {"__index", EffectsLibGet},
  {"__newindex", EffectsLibSet},
  {NULL, NULL}
};
  
}

#endif // DAGON_EFFECTSLIB_H_
