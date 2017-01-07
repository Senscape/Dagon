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

#ifndef DAGON_SYSTEMLIB_H_
#define DAGON_SYSTEMLIB_H_

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by Proxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Control.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

static int SystemLibBrowse(lua_State *L) {
  //System::instance().browse(lua_tostring(L, 1));
  
  return 0;
}

static int SystemLibInit(lua_State *L) {
  // Unused
  return 0;
}

static int SystemLibRun(lua_State *L) {
  Control::instance().run();
  
  return 0;
}

static int SystemLibUpdate(lua_State *L) {
  // We allow this in case the user wants to implement a loop of some kind.
  // FIXME: Currently has a conflict if there's an event hook registered.
  Control::instance().update();
  
  return 0;
}

static int SystemLibTerminate(lua_State *L) {
  Control::instance().initiateTerminate();
  
  return 0;
}

static int SystemShowHelpers(lua_State *L) {
  Config::instance().showHelpers = !Config::instance().showHelpers;

  return 0;
}

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

static const struct luaL_reg SystemLib [] = {
  {"browse", SystemLibBrowse},
  {"init", SystemLibInit},
  {"run", SystemLibRun},
  {"update", SystemLibUpdate},
  {"terminate", SystemLibTerminate},
  {"toggleHelpers", SystemShowHelpers},
  {NULL, NULL}
};

}

#endif // DAGON_SYSTEMLIB_H_
