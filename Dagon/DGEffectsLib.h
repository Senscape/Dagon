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

#ifndef DG_EFFECTSLIB_H
#define DG_EFFECTSLIB_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGEffectsManager.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

static int DGEffectsLibGet(lua_State *L) {
    DGEffectsManager* effectsManager = &DGEffectsManager::getInstance();
    
	const char *key = luaL_checkstring(L, 2);
	
    if (strcmp(key, "brightness") == 0) {
        lua_pushnumber(L, effectsManager->value(DGEffectAdjustBrightness) * 100.0f);
        return 1;
    }
    
	if (strcmp(key, "contrast") == 0) {
        lua_pushnumber(L, effectsManager->value(DGEffectAdjustContrast) * 100.0f);
        return 1;
    }
    
	if (strcmp(key, "dust") == 0) {
        lua_pushnumber(L, effectsManager->value(DGEffectDustIntensity) / 100.0f);
        return 1;
    }
    
	if (strcmp(key, "dustColor") == 0) {
        lua_pushnumber(L, effectsManager->value(DGEffectDustColor));
        return 1;
    }
    
    if (strcmp(key, "dustSize") == 0) {
        lua_pushnumber(L, effectsManager->value(DGEffectDustSize) * 10000.0f);
        return 1;
    }
    
	if (strcmp(key, "dustSpeed") == 0) {
        lua_pushnumber(L, 100.0f - effectsManager->value(DGEffectDustSpeed));
        return 1;
    }
    
    if (strcmp(key, "dustSpread") == 0) {
        lua_pushnumber(L, 100.0f - (effectsManager->value(DGEffectDustSpread) / 10.0f));
        return 1;
    }
    
	if (strcmp(key, "saturation") == 0) {
        lua_pushnumber(L, effectsManager->value(DGEffectAdjustSaturation) * 100.0f);
        return 1;
    }
    
	if (strcmp(key, "motionBlur") == 0) {
        lua_pushnumber(L, (1000.0f - effectsManager->value(DGEffectMotionBlurIntensity) * 100.0f));
        return 1;
    }
    
	if (strcmp(key, "noise") == 0) {
        lua_pushnumber(L, effectsManager->value(DGEffectNoiseIntensity) * 100.0f);
        return 1;
    }
    
	if (strcmp(key, "sepia") == 0) {
        lua_pushnumber(L, effectsManager->value(DGEffectSepiaIntensity) * 100.0f);
        return 1;
    }
    
	if (strcmp(key, "sharpen") == 0) {
        lua_pushnumber(L, effectsManager->value(DGEffectSharpenIntensity) * 100.0f);
        return 1;
    }

    if (strcmp(key, "throb") == 0) {
        lua_pushnumber(L, (100.0f - (effectsManager->value(DGEffectThrobIntensity) * 100.0f)));
        return 1;
    }
    
    if (strcmp(key, "throbStyle") == 0) {
        lua_pushnumber(L, effectsManager->value(DGEffectThrobStyle));
        return 1;
    }
    
	return 0;
}

static int DGEffectsLibSet(lua_State *L) {
    DGEffectsManager* effectsManager = &DGEffectsManager::getInstance();
    
    float value = (float)(luaL_checknumber(L, 3) / 100.0f);
	const char *key = luaL_checkstring(L, 2);
	
	if (strcmp(key, "brightness") == 0) {
        effectsManager->setValuef(DGEffectAdjustBrightness, value);
    }
    
	if (strcmp(key, "contrast") == 0) {
        effectsManager->setValuef(DGEffectAdjustContrast, value);
    }
    
	if (strcmp(key, "saturation") == 0) {
        effectsManager->setValuef(DGEffectAdjustSaturation, value);
    }
    
    if (strcmp(key, "dust") == 0) {
        if (value) {
            effectsManager->setEnabled(DGEffectDust, true);
            effectsManager->setValuef(DGEffectDustIntensity, value * 10000.0f);
        }
        else effectsManager->setEnabled(DGEffectDust, false);
    }
    
    if (strcmp(key, "dustColor") == 0) {
        unsigned int color = lua_tonumber(L, 3);
        effectsManager->setValuei(DGEffectDustColor, color);
    }
    
    if (strcmp(key, "dustSize") == 0) {
        effectsManager->setValuef(DGEffectDustSize, lua_tonumber(L, 3) / 10000.0f);
    }
    
    if (strcmp(key, "dustSpeed") == 0) {
        effectsManager->setValuef(DGEffectDustSpeed, 100.0f - lua_tonumber(L, 3));
    }
    
    if (strcmp(key, "dustSpread") == 0) {
        effectsManager->setValuef(DGEffectDustSpread, 1000.0f - (value * 1000.0f));
    }
    
    // Special case to enable/disable adjustment if three values are normal
    if (effectsManager->value(DGEffectAdjustBrightness) == 1.0f &&
        effectsManager->value(DGEffectAdjustSaturation) == 1.0f &&
        effectsManager->value(DGEffectAdjustContrast) == 1.0f) {
        effectsManager->setEnabled(DGEffectAdjust, false);
    }
    else effectsManager->setEnabled(DGEffectAdjust, true);
    
	if (strcmp(key, "motionBlur") == 0) {
        if (value) {
            effectsManager->setEnabled(DGEffectMotionBlur, true);
            effectsManager->setValuef(DGEffectMotionBlurIntensity, (10.0f - value));
        }
        else effectsManager->setEnabled(DGEffectMotionBlur, false);
    }
    
	if (strcmp(key, "noise") == 0) {
        if (value) {
            effectsManager->setEnabled(DGEffectNoise, true);
            effectsManager->setValuef(DGEffectNoiseIntensity, value);
        }
        else effectsManager->setEnabled(DGEffectNoise, false);
    }
    
	if (strcmp(key, "sepia") == 0) {
        if (value) {
            effectsManager->setEnabled(DGEffectSepia, true);
            effectsManager->setValuef(DGEffectSepiaIntensity, value);
        }
        else effectsManager->setEnabled(DGEffectSepia, false);
    }
    
	if (strcmp(key, "sharpen") == 0) {
        if (value) {
            effectsManager->setEnabled(DGEffectSharpen, true);
            effectsManager->setValuef(DGEffectSharpenIntensity, value);
        }
        else effectsManager->setEnabled(DGEffectSharpen, false);
    }
    
	if (strcmp(key, "throb") == 0) {
        if (value) {
            effectsManager->setEnabled(DGEffectThrob, true);
            effectsManager->setValuef(DGEffectThrobIntensity, (100.0f - (value * 100.0f)));
        }
        else effectsManager->setEnabled(DGEffectThrob, false);
    }
    
	if (strcmp(key, "throbStyle") == 0) {
        effectsManager->setValuef(DGEffectThrobStyle, value * 100.0f);
    }
    
 	return 0;
}

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const struct luaL_Reg DGEffectsLib[] =
{
    {"__index", DGEffectsLibGet},
    {"__newindex", DGEffectsLibSet},
    {NULL, NULL}
};

#endif // DG_EFFECTSLIB_H
