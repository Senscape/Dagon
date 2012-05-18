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

class DGOverlayProxy {
public:
    static const char className[];
    static Luna<DGOverlayProxy>::RegType methods[];
    
    // Constructor
    DGOverlayProxy(lua_State *L) {
        o = new DGOverlay;
        
        o->setName(luaL_checkstring(L, 1));
        
        // Register the new overlay
        DGControl::getInstance().registerObject(o);
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
    
    // Hide the overlay
    int hide(lua_State *L) {
        o->hide();
        
        return 0;
    }
    
    // Check visibility
    int isVisible(lua_State *L) {
        lua_pushboolean(L, o->isVisible());
        return 1;
    }     
    
    // Show the overlay
    int show(lua_State *L) {
        o->show();
        
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
    method(DGOverlayProxy, addButton),      
    method(DGOverlayProxy, addImage),    
    method(DGOverlayProxy, hide),
    method(DGOverlayProxy, isVisible),    
    method(DGOverlayProxy, show),
    {0,0}
};

#endif // DG_OVERLAYPROXY_H
