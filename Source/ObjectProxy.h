////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2013 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
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

// Helper to quickly add the default methods
#define ObjectMethods(proxy) \
method(proxy, disable), \
method(proxy, enable), \
method(proxy, fadeIn), \
method(proxy, fadeOut), \
method(proxy, isEnabled), \
method(proxy, isFading), \
method(proxy, name), \
method(proxy, setFadeSpeed), \
method(proxy, toggle)

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Object.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class ObjectProxy {
public:
    // Constructor
    ObjectProxy() { /* Nothing to do here */ }
    
    // Destructor
    ~ObjectProxy() { /* Nothing to do here */ }
    
    void setObject(Object* caller) {
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
        lua_pushstring(L, o->name().c_str());
        return 1;
    }
    
    // Set speed for fades
    int setFadeSpeed(lua_State *L) {
        o->setFadeSpeed(lua_tonumber(L, 1));
        return 0;
    }
    
    // Toggle enabled/disabled state of the object
    int toggle(lua_State *L) {
        o->toggle();
        return 0;
    }
    
    Object* ptr() { return o; }
    
private:
    Object* o;
    
};

#endif // DG_OBJECTPROXY_H
