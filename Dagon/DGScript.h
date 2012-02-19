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

#ifndef DG_SCRIPT_H
#define DG_SCRIPT_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "DGAction.h"
#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// These are all values we export to Lua to handle directions and types

enum DGLongDirections {
    NORTH = DGNorth,
    EAST = DGEast,
    WEST = DGWest,
    SOUTH = DGSouth,
    UP = DGUp,
    DOWN = DGDown,
    NORTHEAST = DGNorthEast,
    SOUTHEAST = DGSouthEast,
    NORTHWEST = DGNorthWest,
    SOUTHWEST = DGSouthWest
};

enum DGAttachTypes {
    AUDIO = DGObjectAudio,
    CUSTOM = DGActionCustom,
    IMAGE = DGObjectTexture,
    FEEDBACK = DGActionFeedback,
    SWITCH = DGActionSwitch,
    VIDEO = DGObjectVideo
};

// Helper to push values to Lua
#define DGLuaEnum(L, name, val) \
lua_pushnumber(L, val); \
lua_setfield(L, -2, #name);

class DGConfig;
class DGLog;
class DGSystem;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGScript {
    DGConfig* config;
    DGLog* log;
    DGSystem* system;
    
    std::vector<std::string> _arrayOfModuleNames;
    bool _isInitialized;
    lua_State* _L;
    
    int _ref;
    void _error(int result);
    static int _globalRoom(lua_State *L);
    static int _globalSwitch(lua_State *L);
    static int _globalTimer(lua_State *L);    
    void _registerGlobals();
    
    // Private constructor/destructor
    DGScript();
    ~DGScript();
    // Stop the compiler generating methods of copy the object
    DGScript(DGScript const& copy);            // Not implemented
    DGScript& operator=(DGScript const& copy); // Not implemented
    
public:
    static DGScript& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGScript instance;
        return instance;
    }

    void init(int argc, char* argv[]);
    void callback(int handler, int object);
    const char* module();
    bool isExecutingModule();
    void run();
    void setModule(const char* module);
    void unsetModule();
};

#endif // DG_SCRIPT_H
