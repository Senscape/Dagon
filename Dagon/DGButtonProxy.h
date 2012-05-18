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

#ifndef DG_BUTTONPROXY_H
#define DG_BUTTONPROXY_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGButton.h"
#include "DGScript.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGButtonProxy {
public:
    static const char className[];
    static Luna<DGButtonProxy>::RegType methods[];
    
    // Constructor
    DGButtonProxy(lua_State *L) {
        b = new DGButton();
        
        b->setPosition(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        b->setSize(luaL_checknumber(L, 3), luaL_checknumber(L, 4));
    }
    
    // Destructor
    ~DGButtonProxy() { delete b; }
    
    // Move the button
    int move(lua_State *L) {
        b->move(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        return 0;
    }
    
    // Return the position
    int position(lua_State *L) {
        DGPoint position = b->position();
        lua_pushnumber(L, position.x);
        lua_pushnumber(L, position.y);        
        return 2;
    }
    
    // Scale the button
    int scale(lua_State *L) {
        b->scale(luaL_checknumber(L, 1));
        return 0;
    }
    
    // Set an action
    int setAction(lua_State *L) {
        DGAction action;
        
        int type = (int)luaL_checknumber(L, 1);
        
        switch (type) {
            case CUSTOM:
                if (!lua_isfunction(L, 2)) {
                    DGLog::getInstance().error(DGModScript, "%s", DGMsg250006);
                    return 0;
                }
                
                action.type = DGActionCustom;
                action.luaHandler = luaL_ref(L, LUA_REGISTRYINDEX); // Pop and return a reference to the table
                
                b->setAction(&action);
                if (!b->hasColor())
                    b->setColor(0);
                
                break;
            case FEED:
                action.type = DGActionFeed;
                strncpy(action.feed, luaL_checkstring(L, 2), DGMaxFeedLength);
                
                b->setAction(&action);
                if (!b->hasColor())
                    b->setColor(0);
                
                break;
            case SWITCH:
                int type = DGCheckProxy(L, 2);
                if (type == DGObjectNode)
                    action.target = DGProxyToNode(L, 2);
                else if (type == DGObjectRoom)
                    action.target = DGProxyToRoom(L, 2);
                else {
                    DGLog::getInstance().error(DGModScript, "%s", DGMsg250005);
                    return 0;
                }
                
                action.type = DGActionSwitch;
                
                b->setAction(&action);
                if (!b->hasColor())
                    b->setColor(0);
                
                break;
        }
                
        return 0;
    }
    
    // Set the background image
    int setImage(lua_State *L) {
        b->setTexture(luaL_checkstring(L, 1));
        return 0;
    }    
    
    // Set a new position
    int setPosition(lua_State *L) {
        b->setPosition(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        return 0;
    }
    
    // Set a new size
    int setSize(lua_State *L) {
        b->setSize(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        return 0;
    }
    
    // Return the size
    int size(lua_State *L) {
        DGSize size = b->size();
        lua_pushnumber(L, size.width);
        lua_pushnumber(L, size.height);        
        return 2;
    }
    
    DGButton* ptr() { return b; }
    
private:
    DGButton* b;
    
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char DGButtonProxy::className[] = DGButtonProxyName;

Luna<DGButtonProxy>::RegType DGButtonProxy::methods[] = {
    method(DGButtonProxy, move),
    method(DGButtonProxy, position),
    method(DGButtonProxy, scale),
    method(DGButtonProxy, setAction),      
    method(DGButtonProxy, setImage),    
    method(DGButtonProxy, setPosition),
    method(DGButtonProxy, setSize),
    method(DGButtonProxy, size),    
    {0,0}
};

#endif // DG_BUTTONPROXY_H
