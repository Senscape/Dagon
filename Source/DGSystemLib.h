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

#ifndef DG_SYSTEMLIB_H
#define DG_SYSTEMLIB_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGControl.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

static int DGSystemLibBrowse(lua_State *L) {
  //DGSystem::instance().browse(lua_tostring(L, 1));
  
  return 0;
}

static int DGSystemLibInit(lua_State *L) {
  DGControl::instance().init();
  
  return 0;
}

static int DGSystemLibRun(lua_State *L) {
  DGControl::instance().run();
  
  return 0;
}

static int DGSystemLibUpdate(lua_State *L) {
  // We allow this in case the user wants to implement a loop of some kind.
  // FIXME: Currently has a conflict if there's an event hook registered.
  DGControl::instance().update();
  
  return 0;
}

static int DGSystemLibTerminate(lua_State *L) {
  DGControl::instance().terminate();
  
  return 0;
}

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

static const struct luaL_reg DGSystemLib [] = {
  {"browse", DGSystemLibBrowse},
  {"init", DGSystemLibInit},
  {"run", DGSystemLibRun},
  {"update", DGSystemLibUpdate},
  {"terminate", DGSystemLibTerminate},
  {NULL, NULL}
};

#endif // DG_SYSTEMLIB_H
