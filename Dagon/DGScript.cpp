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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <string.h>
#include "DGConfig.h"
#include "DGControl.h"
#include "DGLanguage.h"
#include "DGProxy.h"
#include "DGScript.h"
#include "DGSystemLib.h"
#include "Luna.h"

using namespace std;

// The way the engine is designed, all static Lua functions will
// have to grab a reference to the control singleton and log if necessary
// No performance hit but something to keep in mind

// Spots launch audio and video, then perform a switch in that order
// To avoid doing too many customs

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGScript::DGScript() {
    _isInitialized = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGScript::~DGScript() {
    if (_isInitialized)
        lua_close(_L);
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

// TODO: Support loading script from parameters
void DGScript::init(int argc, char* argv[]) {
    char script[DGMaxFileLength];
    
    // First thing we do is get the paths to load the script
    // (note that it's not necessary to init the system)
    DGSystem::getInstance().findPaths(argc, argv);
    
    // If autorun is enabled, automatically init the system
    if (DGConfig::getInstance().autorun)
        DGSystem::getInstance().init();
    
    _L = lua_open();
    luaL_openlibs(_L);
    
    Luna<DGNodeProxy>::Register(_L);
    Luna<DGRoomProxy>::Register(_L);
    luaL_register(_L, "system", DGSystemLib);
    
    _registerGlobals();
    
    snprintf(script, DGMaxFileLength, "%s.lua", DGConfig::getInstance().script());
    if (luaL_loadfile(_L, script) == 0)
        _isInitialized = true;
    else
        DGLog::getInstance().error(DGModScript, "%s: %s", DGMsg250003, script);
}

const char* DGScript::module() {
    return _arrayOfModuleNames.back().c_str();
}

bool DGScript::isExecutingModule() {
    // We simply test if the vector is empty. If it is,
    // it means no modules are stacked.
    return !_arrayOfModuleNames.empty();
}

void DGScript::run() {
    if (_isInitialized) {
        lua_pcall(_L, 0, 0, 0);
        
        // Check if we must start the main loop ourselves
        if (DGConfig::getInstance().autorun)
            DGSystem::getInstance().run();
    }
}

void DGScript::setModule(const char* module) {
    _arrayOfModuleNames.push_back(module);
}

void DGScript::unsetModule() {
    _arrayOfModuleNames.pop_back();
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

int DGScript::_globalRoom(lua_State *L) {
    // NOTE: This is a convenience Lua hack, which in theory is 100% safe.
    // Must do losts of testing. Basically, this helper creates a room instance,
    // loads the corresponding Lua file and makes sure it's never loaded twice.
    
    char module[DGMaxObjectName];
    strncpy(module, luaL_checkstring(L, 1), DGMaxObjectName);
    
    // We first check if the object already exists
    lua_getglobal(L, module);
    if (!lua_isuserdata(L, -1)) {
        char line[DGMaxLogLength], script[DGMaxFileLength];
        
        // Parse line to automatically create the room
        // TODO: Decide later if we should enforce the user to manually create the room
        snprintf(line, DGMaxLogLength, "%s = Room(\"%s\")", module, module);
        luaL_dostring(L, line);
        
        // Load the corresponding Lua file
        // TODO: Read rooms from path
        snprintf(script, DGMaxFileLength, "%s.lua", module);
        
        if (luaL_loadfile(L, script) == 0) {
            DGScript::getInstance().setModule(module);
            lua_pcall(L, 0, 0, 0);
            DGScript::getInstance().unsetModule();
        }
    }
    
    // Nothing to do...
    
    return 0;
}

int DGScript::_globalSwitch(lua_State *L) {
    switch (DGCheckProxy(L, 1)) {
        case DGObjectNode:
            DGControl::getInstance().switchTo(DGProxyToNode(L, 1));
            break;
        case DGObjectRoom:
            DGControl::getInstance().switchTo(DGProxyToRoom(L, 1));
            break;
        case DGObjectGeneric:
            DGLog::getInstance().error(DGModScript, "%s", DGMsg250000);
            break;
            
        case DGObjectNone:
            DGLog::getInstance().error(DGModScript, "%s", DGMsg250001);
            break;
    }
    
	return 0;
}

void DGScript::_registerGlobals() {
    static const struct luaL_reg globalLibs [] = {
        {"room", _globalRoom},
        {"switch", _globalSwitch},
        {NULL, NULL}
    };
    
    lua_getglobal(_L, "_G");
	luaL_register(_L, NULL, globalLibs);
	lua_pop(_L, 1);
}
