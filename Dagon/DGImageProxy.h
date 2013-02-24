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

#ifndef DG_IMAGEPROXY_H
#define DG_IMAGEPROXY_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGImage.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGImageProxy : public DGObjectProxy {
public:
    static const char className[];
    static Luna<DGImageProxy>::RegType methods[];
    
    // Constructor
    DGImageProxy(lua_State *L) {
        i = new DGImage(luaL_checkstring(L, 1));
        i->setName(luaL_checkstring(L, 1));
        
        // Init the base class
        this->setObject(i);
    }
    
    // Destructor
    ~DGImageProxy() { delete i; }
    
    // Move the image
    int move(lua_State *L) {
        i->move(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        return 0;
    }
    
    // Return the position
    int position(lua_State *L) {
        DGPoint position = i->position();
        lua_pushnumber(L, position.x);
        lua_pushnumber(L, position.y);        
        return 2;
    }
    
    // Scale the image
    int scale(lua_State *L) {
        i->scale(luaL_checknumber(L, 1));
        return 0;
    }
    
    // Set a new position
    int setPosition(lua_State *L) {
        i->setPosition(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        return 0;
    }
    
    // Set a new size
    int setSize(lua_State *L) {
        i->setSize(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        return 0;
    }
    
    // Return the size
    int size(lua_State *L) {
        DGSize size = i->size();
        lua_pushnumber(L, size.width);
        lua_pushnumber(L, size.height);        
        return 2;
    }
    
    DGImage* ptr() { return i; }
    
private:
    DGImage* i;
    
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char DGImageProxy::className[] = DGImageProxyName;

Luna<DGImageProxy>::RegType DGImageProxy::methods[] = {
    DGObjectMethods(DGImageProxy),   
    method(DGImageProxy, move),
    method(DGImageProxy, position),
    method(DGImageProxy, scale),
    method(DGImageProxy, setPosition),
    method(DGImageProxy, setSize),      
    method(DGImageProxy, size),
    {0,0}
};

#endif // DG_IMAGEPROXY_H
