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

#include <string>
#include <vector>
#include "DGDefines.h"
#include "DGLog.h"

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGScript {
    std::vector<std::string> _arrayOfModuleNames;
    bool _isInitialized;
    lua_State* _L;
    
    static int _globalRoom(lua_State *L);
    static int _globalSwitch(lua_State *L);
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

    void init();
    const char* module();
    bool isExecutingModule();
    void run();
    void setModule(const char* module);
    void unsetModule();
};

#endif // DG_SCRIPT_H
