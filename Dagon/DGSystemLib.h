////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2012 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
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
#include "DGSystem.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

static int DGSystemLibBrowse(lua_State *L) {
    DGSystem::getInstance().browse(lua_tostring(L, 1));
    
    return 0;
}

static int DGSystemLibInit(lua_State *L) {
    DGSystem::getInstance().init();
    
    return 0;
}

static int DGSystemLibRun(lua_State *L) {
    DGSystem::getInstance().run();
	
	return 0;
}

static int DGSystemLibUpdate(lua_State *L) {
    // We allow this in case the user wants to implement a loop of some kind.
    // FIXME: Currently has a conflict if there's an event hook registered.
    DGControl::getInstance().update();
	
	return 0;
}

static int DGSystemLibTerminate(lua_State *L) {
    DGControl::getInstance().terminate();
	
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
