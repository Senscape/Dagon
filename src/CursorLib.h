////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DAGON_CURSORLIB_H_
#define DAGON_CURSORLIB_H_

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by Proxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "CursorManager.h"

namespace dagon {

// FIXME: The cursor manager shouldn't inherit from Object...
// In fact, it shouldn't be a singleton.

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

static int CursorLibDisable(lua_State *L) {
  CursorManager::instance().Object::disable();
  
  return 0;
}

static int CursorLibEnable(lua_State *L) {
  CursorManager::instance().Object::enable();
  
  return 0;
}

static int CursorLibFadeIn(lua_State *L) {
  CursorManager::instance().Object::fadeIn();
  
  return 0;
}

static int CursorLibFadeOut(lua_State *L) {
  CursorManager::instance().Object::fadeOut();
  
  return 0;
}

static int CursorLibLoad(lua_State *L) {
  CursorManager::instance().load(static_cast<int>(luaL_checknumber(L, 1)),
                                   luaL_checkstring(L, 2),
                                   static_cast<int>(lua_tonumber(L, 3)),
                                   static_cast<int>(lua_tonumber(L, 4)));
  
  return 0;
}

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

static const struct luaL_reg kCursorLib [] = {
  {"disable", CursorLibDisable},
  {"enable", CursorLibEnable},
  {"fadeIn", CursorLibFadeIn},
  {"fadeOut", CursorLibFadeOut},
  {"load", CursorLibLoad},
  {NULL, NULL}
};
  
}

#endif // DAGON_CURSORLIB_H_
