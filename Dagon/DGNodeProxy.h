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

#ifndef DG_NODEPROXY_H
#define DG_NODEPROXY_H

////////////////////////////////////////////////////////////
// NOTE: This header file should not be included directly.
// It's referenced by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGControl.h"
#include "DGNode.h"
#include "DGRoom.h"
#include "DGScript.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGNodeProxy {
public:
    static const char className[];
    static Luna<DGNodeProxy>::RegType methods[];
    
    // Constructor
    DGNodeProxy(lua_State *L) { 
        n = new DGNode;
        n->setBundleName(luaL_checkstring(L, 1)); // If more than one parameter, should be the name of the object
        
        if (lua_gettop(L) == 2)
            n->setName(luaL_checkstring(L, 2));
        else
            n->setName(luaL_checkstring(L, 1));
        
        // Register the node in the controller (generates the bundle)
        DGControl::getInstance().registerObject(n);
        
        if (DGScript::getInstance().isExecutingModule()) {
            // A module is being executed, so we know a room
            // object of the same name exists
            
            lua_getglobal(L, DGScript::getInstance().module());
            DGRoom* r = DGProxyToRoom(L, -1);
            
            // We add the node automatically
            r->addNode(n);
        }
    }
    
    // Destructor
    ~DGNodeProxy() { delete n; }
    
    // Function to link in several directions
    int link(lua_State *L) {
        if (!lua_istable(L, 1)) {
            luaL_error(L, DGMsg250002);
            
            return 0;
        }
        
        lua_pushnil(L);
        while (lua_next(L, 1) != 0) {
            int dir;
            const char* key = lua_tostring(L, -2);
            
            // Ugly nesting of strcmps() but there's probably no better way of doing this...
            if (strcmp(key, "N") == 0) dir = DGNorth;
            else if (strcmp(key, "W") == 0) dir = DGWest;
            else if (strcmp(key, "S") == 0) dir = DGSouth;
            else if (strcmp(key, "E") == 0) dir = DGEast;
            else if (strcmp(key, "NW") == 0) dir = DGNorthWest;
            else if (strcmp(key, "SW") == 0) dir = DGSouthWest;
            else if (strcmp(key, "SE") == 0) dir = DGSouthEast;
            else if (strcmp(key, "NE") == 0) dir = DGNorthEast;
            
            if (lua_isfunction(L, -1)) {
                int ref = luaL_ref(L, LUA_REGISTRYINDEX);
                n->addCustomLink(dir, ref);
            }
            else {
                switch (DGCheckProxy(L, 3)) {
                    case DGObjectNode:
                        n->addLink(dir, (DGObject*)DGProxyToNode(L, 3));
                        break;
                    case DGObjectRoom:
                        n->addLink(dir, (DGObject*)DGProxyToRoom(L, 3));
                        break;
                }
            }
            
            lua_pop(L, 1);
        }
        
        return 0;
        
    }
    
    DGNode* ptr() { return n; }
    
private:
    DGNode* n;    
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char DGNodeProxy::className[] = DGNodeProxyName;

Luna<DGNodeProxy>::RegType DGNodeProxy::methods[] = {
    method(DGNodeProxy, link),
    {0,0}
};

#endif // DG_NODEPROXY_H
