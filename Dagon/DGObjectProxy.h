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

#ifndef DG_OBJECTPROXY_H
#define DG_OBJECTPROXY_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// Helper quickly add the default methods
#define DGObjectMethods(proxy) \
method(proxy, disable), \
method(proxy, enable), \
method(proxy, fadeIn), \
method(proxy, fadeOut), \
method(proxy, isEnabled), \
method(proxy, isFading), \
method(proxy, name), \
method(proxy, toggle),

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGObject.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGObjectProxy {
public:
    // Constructor
    DGObjectProxy() { /* Nothing to do here */ }
    
    // Destructor
    ~DGObjectProxy() { /* Nothing to do here */ }
    
    void setObject(DGObject* caller) {
        o = caller;
    }
    
    // Disable the object
    int disable(lua_State *L) {
        o->disable();
        return 0;
    }
    
    // Enable the object
    int enable(lua_State *L) {
        o->enable();
        return 0;
    }
    
    // Fade in
    int fadeIn(lua_State *L) {
        o->fadeIn();
        return 0;
    }
    
    // Fade out
    int fadeOut(lua_State *L) {
        o->fadeOut();
        return 0;
    }
    
    // Check if fading
    int isFading(lua_State *L) {
        lua_pushboolean(L, o->isFading());
        return 1;
    }
    
    // Check if enabled
    int isEnabled(lua_State *L) {
        lua_pushboolean(L, o->isEnabled());
        return 1;
    }
    
    // Return the name
    int name(lua_State *L) {
        lua_pushstring(L, o->name());
        return 1;
    }
    
    // Toggle enabled/disabled state of the object
    int toggle(lua_State *L) {
        o->toggle();
        return 0;
    }
    
    DGObject* ptr() { return o; }
    
private:
    DGObject* o;
    
};

#endif // DG_OBJECTPROXY_H
