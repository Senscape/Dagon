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

#ifndef DG_OVERLAYPROXY_H
#define DG_OVERLAYPROXY_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGControl.h"
#include "DGOverlay.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGOverlayProxy : public DGObjectProxy {
public:
    static const char className[];
    static Luna<DGOverlayProxy>::RegType methods[];
    
    // Constructor
    DGOverlayProxy(lua_State *L) {
        o = new DGOverlay;
        
        o->setName(luaL_checkstring(L, 1));
        
        // Register the new overlay
        DGControl::getInstance().registerObject(o);
        
        // Init the base class
        this->setObject(o);
    }
    
    // Destructor
    ~DGOverlayProxy() { delete o; }
    
    // Add a button to the overlay
    int addButton(lua_State *L) {
        if (DGCheckProxy(L, 1) == DGObjectButton) {
            o->addButton(DGProxyToButton(L, 1));
            
            // Now we get the metatable of the added spot and set it
            // as a return value
            lua_getfield(L, LUA_REGISTRYINDEX, DGButtonProxyName);
            lua_setmetatable(L, -2);
            
            return 1;
        }
        
        return 0;
    } 
    
    // Add an image to the overlay
    int addImage(lua_State *L) {
        if (DGCheckProxy(L, 1) == DGObjectImage) {
            o->addImage(DGProxyToImage(L, 1));
            
            // Now we get the metatable of the added spot and set it
            // as a return value
            lua_getfield(L, LUA_REGISTRYINDEX, DGImageProxyName);
            lua_setmetatable(L, -2);
            
            return 1;
        }
        
        return 0;
    }
    
    // Fadein (overrides object method)
    int fadeIn(lua_State *L) {
        o->fadeIn();
        
        return 0;
    }
    
    // Fadeout (overrides object method)
    int fadeOut(lua_State *L) {
        o->fadeOut();
        
        return 0;
    }
    
    // Move the overlay
    int move(lua_State *L) {
        o->move(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        
        return 0;
    } 
    
    // Return the position
    int position(lua_State *L) {
        DGPoint position = o->position();
        lua_pushnumber(L, position.x);
        lua_pushnumber(L, position.y);        
        return 2;
    }
    
    // Set a new position
    int setPosition(lua_State *L) {
        o->setPosition(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        
        return 0;
    } 

    DGOverlay* ptr() { return o; }
    
private:
    DGOverlay* o;
    
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char DGOverlayProxy::className[] = DGOverlayProxyName;

Luna<DGOverlayProxy>::RegType DGOverlayProxy::methods[] = {
    DGObjectMethods(DGOverlayProxy),    
    method(DGOverlayProxy, addButton),      
    method(DGOverlayProxy, addImage),
    method(DGOverlayProxy, fadeIn),
    method(DGOverlayProxy, fadeOut),
    method(DGOverlayProxy, move),
    method(DGOverlayProxy, position),    
    method(DGOverlayProxy, setPosition),
    {0,0}
};

#endif // DG_OVERLAYPROXY_H
