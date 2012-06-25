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
#include "DGConfig.h"
#include "DGControl.h"
#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum DGAttachTypes {
    AUDIO = DGObjectAudio,
    FUNCTION = DGActionFunction,
    IMAGE = DGObjectTexture,
    FEED = DGActionFeed,
    SWITCH = DGActionSwitch,
    VIDEO = DGObjectVideo
};

// Helper to push values to Lua
#define DGLuaEnum(L, name, val) \
lua_pushnumber(L, val); \
lua_setfield(L, -2, #name);

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
    bool _isSuspended;    
    lua_State* _L;
    lua_State* _thread;
    
    int _ref;
    void _error(int result);
    static int _globalFeed(lua_State *L);
    static int _globalLookAt(lua_State *L);    
    static int _globalLoadCursor(lua_State *L);
    static int _globalPlay(lua_State *L);    
    static int _globalRegister(lua_State *L);   
    static int _globalRoom(lua_State *L);
    static int _globalSetFont(lua_State *L);    
    static int _globalSleep(lua_State *L);       
    static int _globalSwitch(lua_State *L);
    static int _globalStartTimer(lua_State *L); 
    static int _globalStopTimer(lua_State *L);
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
    
    void execute();
    void init(int argc, char* argv[]);
    const char* module();
    bool isExecutingModule();    
    void processCallback(int handler, int object);    
    void processCommand(const char* command);
    void resume();
    void run();
    void setModule(const char* module);
    int suspend();
    void unsetModule();
};

#endif // DG_SCRIPT_H
