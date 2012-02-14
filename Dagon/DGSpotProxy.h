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

#ifndef DG_SPOTPROXY_H
#define DG_SPOTPROXY_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGSpot.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGSpotProxy {
public:
    static const char className[];
    static Luna<DGSpotProxy>::RegType methods[];
    
    // Constructor
    DGSpotProxy(lua_State *L) {
        if (lua_istable(L, 2)) {
            std::vector<int> arrayOfCoords;
            
            lua_pushnil(L);  // First key
            while (lua_next(L, 2) != 0) {
                // Uses key at index -2 and value at index -1
                arrayOfCoords.push_back(lua_tonumber(L, -1));
                lua_pop(L, 1);
            }
            
            s = new DGSpot(arrayOfCoords, luaL_checknumber(L, 1), 0);
        }
        else luaL_error(L, DGMsg250004);
    }
    
    // TODO: In the future we should return a pointer to an attached object
    int attach(lua_State *L) {
        int type = (int)luaL_checknumber(L, 1);
        
        switch (type) {
            case 0:
                if (lua_isfunction(L, 2)) {
                    
                    return 0;
                }
                
               /* ref = luaL_ref(L, LUA_REGISTRYINDEX);  // pop and return a reference to the table.
                
                action.type = DG_ACTION_CUSTOM;
                action.custom_handler = ref;
                
                DGSpotSetAction(checkobject(L, DG_OBJECT_SPOT, 1), &action);
                if (!DGSpotHasColor(spot))
                    DGSpotSetColor(spot, 0);*/
                
                break;
        }

        return 0;
    }
    
    // Destructor
    ~DGSpotProxy() { delete s; }
    
    DGSpot* ptr() { return s; }
    
private:
    DGSpot* s;    
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char DGSpotProxy::className[] = DGSpotProxyName;

Luna<DGSpotProxy>::RegType DGSpotProxy::methods[] = {
    method(DGSpotProxy, attach),
    {0,0}
};

#endif // DG_SPOTPROXY_H
