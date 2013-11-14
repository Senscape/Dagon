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
    lua_pushnumber(L, effectsManager.value(kEffectBrightness) * 100.0f);
    return 1;
  }
  
  if (strcmp(key, "contrast") == 0) {
    lua_pushnumber(L, effectsManager.value(kEffectContrast) * 100.0f);
    return 1;
  }
  
  if (strcmp(key, "dust") == 0) {
    lua_pushnumber(L, effectsManager.value(kEffectDust) / 100.0f);
    return 1;
  }
  
  if (strcmp(key, "dustColor") == 0) {
    lua_pushnumber(L, effectsManager.value(kEffectDustColor));
    return 1;
  }
  
  if (strcmp(key, "dustSize") == 0) {
    lua_pushnumber(L, effectsManager.value(kEffectDustSize) * 10000.0f);
    return 1;
  }
  
  if (strcmp(key, "dustSpeed") == 0) {
    lua_pushnumber(L, 100.0f - effectsManager.value(kEffectDustSpeed));
    return 1;
  }
  
  if (strcmp(key, "dustSpread") == 0) {
    lua_pushnumber(L, 100.0f - (effectsManager.value(kEffectDustSpread) / 10.0f));
    return 1;
  }
  
  if (strcmp(key, "saturation") == 0) {
    lua_pushnumber(L, effectsManager.value(kEffectSaturation) * 100.0f);
    return 1;
  }
  
  if (strcmp(key, "motionBlur") == 0) {
    lua_pushnumber(L, (1000.0f - effectsManager.value(kEffectMotionBlur) * 100.0f));
    return 1;
  }
  
  if (strcmp(key, "noise") == 0) {
    lua_pushnumber(L, effectsManager.value(kEffectNoise) * 100.0f);
    return 1;
  }
  
  if (strcmp(key, "sepia") == 0) {
    lua_pushnumber(L, effectsManager.value(kEffectSepia) * 100.0f);
    return 1;
  }
  
  if (strcmp(key, "sharpen") == 0) {
    lua_pushnumber(L, effectsManager.value(kEffectSharpen) * 100.0f);
    return 1;
  }
  
  if (strcmp(key, "throb") == 0) {
    lua_pushnumber(L, (100.0f - (effectsManager.value(kEffectThrob) * 100.0f)));
    return 1;
  }
  
  if (strcmp(key, "throbStyle") == 0) {
    lua_pushnumber(L, effectsManager.value(kEffectThrobStyle));
    return 1;
  }
  
  return 0;
}

static int EffectsLibSet(lua_State *L) {
  EffectsManager& effectsManager = EffectsManager::instance();
  
  float value = (float)(luaL_checknumber(L, 3) / 100.0f);
  const char *key = luaL_checkstring(L, 2);
  
  if (strcmp(key, "brightness") == 0) {
    effectsManager.setValuef(kEffectBrightness, value);
  }
  
  if (strcmp(key, "contrast") == 0) {
    effectsManager.setValuef(kEffectContrast, value);
  }
  
  if (strcmp(key, "saturation") == 0) {
    effectsManager.setValuef(kEffectSaturation, value);
  }
  
  if (strcmp(key, "dust") == 0) {
    effectsManager.setValuef(kEffectDust, value * 10000.0f);
  }
  
  if (strcmp(key, "dustColor") == 0) {
    effectsManager.setValuei(kEffectDustColor,
                             static_cast<uint32_t>(lua_tonumber(L, 3)));
  }
  
  if (strcmp(key, "dustSize") == 0) {
    effectsManager.setValuef(kEffectDustSize, (float)lua_tonumber(L, 3) / 10000.0f);
  }
  
  if (strcmp(key, "dustSpeed") == 0) {
    effectsManager.setValuef(kEffectDustSpeed,
                             100.0f - (float)lua_tonumber(L, 3));
  }
  
  if (strcmp(key, "dustSpread") == 0) {
    effectsManager.setValuef(kEffectDustSpread, 1000.0f - (value * 1000.0f));
  }
  
  if (strcmp(key, "motionBlur") == 0) {
    effectsManager.setValuef(kEffectMotionBlur, (10.0f - value));
  }
  
  if (strcmp(key, "noise") == 0) {
    effectsManager.setValuef(kEffectNoise, value);
  }
  
  if (strcmp(key, "sepia") == 0) {
    effectsManager.setValuef(kEffectSepia, value);
  }
  
  if (strcmp(key, "sharpen") == 0) {
    effectsManager.setValuef(kEffectSharpen, value);
  }
  
  if (strcmp(key, "throb") == 0) {
    effectsManager.setValuef(kEffectThrob, (100.0f - (value * 100.0f)));
  }
  
  if (strcmp(key, "throbStyle") == 0) {
    effectsManager.setValuef(kEffectThrobStyle, value * 100.0f);
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
