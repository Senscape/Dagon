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
  
  if (strcmp(key, "brightness") == 0) {
    lua_pushnumber(L, effectsManager.value(effects::kBrightness));
    return 1;
  }
  
  if (strcmp(key, "contrast") == 0) {
    lua_pushnumber(L, effectsManager.value(effects::kContrast));
    return 1;
  }
  
  if (strcmp(key, "dust") == 0) {
    lua_pushnumber(L, effectsManager.value(effects::kDust));
    return 1;
  }
  
  if (strcmp(key, "dustColor") == 0) {
    lua_pushnumber(L, effectsManager.value(effects::kDustColor));
    return 1;
  }
  
  if (strcmp(key, "dustSize") == 0) {
    lua_pushnumber(L, effectsManager.value(effects::kDustSize));
    return 1;
  }
  
  if (strcmp(key, "dustSpeed") == 0) {
    lua_pushnumber(L, 100.0f - effectsManager.value(effects::kDustSpeed));
    return 1;
  }
  
  if (strcmp(key, "dustSpread") == 0) {
    lua_pushnumber(L, effectsManager.value(effects::kDustSpread));
    return 1;
  }
  
  if (strcmp(key, "saturation") == 0) {
    lua_pushnumber(L, effectsManager.value(effects::kSaturation));
    return 1;
  }
  
  if (strcmp(key, "motionBlur") == 0) {
    lua_pushnumber(L, effectsManager.value(effects::kMotionBlur));
    return 1;
  }
  
  if (strcmp(key, "noise") == 0) {
    lua_pushnumber(L, effectsManager.value(effects::kNoise));
    return 1;
  }
  
  if (strcmp(key, "sepia") == 0) {
    lua_pushnumber(L, effectsManager.value(effects::kSepia));
    return 1;
  }
  
  if (strcmp(key, "sharpen") == 0) {
    lua_pushnumber(L, effectsManager.value(effects::kSharpen));
    return 1;
  }
  
  if (strcmp(key, "throb") == 0) {
    lua_pushnumber(L, effectsManager.value(effects::kThrob));
    return 1;
  }
  
  if (strcmp(key, "throbStyle") == 0) {
    lua_pushnumber(L, effectsManager.value(effects::kThrobStyle));
    return 1;
  }
  
  return 0;
}

static int EffectsLibSet(lua_State *L) {
  EffectsManager& effectsManager = EffectsManager::instance();
  
  int value = (float)(luaL_checknumber(L, 3));
  const char *key = luaL_checkstring(L, 2);
  
  if (strcmp(key, "brightness") == 0) {
    effectsManager.setValuef(effects::kBrightness, value);
  }
  
  if (strcmp(key, "contrast") == 0) {
    effectsManager.setValuef(effects::kContrast, value);
  }
  
  if (strcmp(key, "saturation") == 0) {
    effectsManager.setValuef(effects::kSaturation, value);
  }
  
  if (strcmp(key, "dust") == 0) {
    effectsManager.setValuef(effects::kDust, value);
  }
  
  if (strcmp(key, "dustColor") == 0) {
    effectsManager.setValuei(effects::kDustColor,
                             static_cast<uint32_t>(lua_tonumber(L, 3)));
  }
  
  if (strcmp(key, "dustSize") == 0) {
    effectsManager.setValuef(effects::kDustSize, value);
  }
  
  if (strcmp(key, "dustSpeed") == 0) {
    effectsManager.setValuef(effects::kDustSpeed, value);
  }
  
  if (strcmp(key, "dustSpread") == 0) {
    effectsManager.setValuef(effects::kDustSpread, value);
  }
  
  if (strcmp(key, "motionBlur") == 0) {
    effectsManager.setValuef(effects::kMotionBlur, value);
  }
  
  if (strcmp(key, "noise") == 0) {
    effectsManager.setValuef(effects::kNoise, value);
  }
  
  if (strcmp(key, "sepia") == 0) {
    effectsManager.setValuef(effects::kSepia, value);
  }
  
  if (strcmp(key, "sharpen") == 0) {
    effectsManager.setValuef(effects::kSharpen, value);
  }
  
  if (strcmp(key, "throb") == 0) {
    effectsManager.setValuef(effects::kThrob, value);
  }
  
  if (strcmp(key, "throbStyle") == 0) {
    effectsManager.setValuef(effects::kThrobStyle, value);
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
