////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DAGON_OVERLAYPROXY_H_
#define DAGON_OVERLAYPROXY_H_

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by Proxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Control.h"
#include "Overlay.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class OverlayProxy : public ObjectProxy {
public:
  static const char className[];
  static Luna<OverlayProxy>::RegType methods[];
  
  // Constructor
  OverlayProxy(lua_State *L) {
    o = new Overlay;
    
    o->setName(luaL_checkstring(L, 1));
    
    // Register the new overlay
    Control::instance().registerObject(o);
    
    // Init the base class
    this->setObject(o);
  }
  
  // Destructor
  ~OverlayProxy() { delete o; }
  
  // Add a button to the overlay
  int addButton(lua_State *L) {
    if (DGCheckProxy(L, 1) == kObjectButton) {
      o->addButton(ProxyToButton(L, 1));
      
      // Now we get the metatable of the added spot and set it
      // as a return value
      lua_getfield(L, LUA_REGISTRYINDEX, ButtonProxyName);
      lua_setmetatable(L, -2);
      
      return 1;
    }
    
    return 0;
  }
  
  // Add an image to the overlay
  int addImage(lua_State *L) {
    if (DGCheckProxy(L, 1) == kObjectImage) {
      o->addImage(ProxyToImage(L, 1));
      
      // Now we get the metatable of the added spot and set it
      // as a return value
      lua_getfield(L, LUA_REGISTRYINDEX, ImageProxyName);
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
    o->move(static_cast<int>(luaL_checknumber(L, 1)),
            static_cast<int>(luaL_checknumber(L, 2)));
    
    return 0;
  }
  
  // Return the position
  int position(lua_State *L) {
    Point newPosition = o->position();
    lua_pushnumber(L, newPosition.x);
    lua_pushnumber(L, newPosition.y);
    return 2;
  }
  
  // Set a new position
  int setPosition(lua_State *L) {
    o->setPosition(static_cast<int>(luaL_checknumber(L, 1)),
                   static_cast<int>(luaL_checknumber(L, 2)));
    
    return 0;
  }

  int setDefaultCursor(lua_State *L) {
    o->setDefaultCursor(static_cast<int>(luaL_checknumber(L, 1)));
    return 0;
  }
  
  Overlay* ptr() { return o; }
  
private:
  Overlay* o;
  
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char OverlayProxy::className[] = OverlayProxyName;

Luna<OverlayProxy>::RegType OverlayProxy::methods[] = {
  ObjectMethods(OverlayProxy),
  method(OverlayProxy, addButton),
  method(OverlayProxy, addImage),
  method(OverlayProxy, fadeIn),
  method(OverlayProxy, fadeOut),
  method(OverlayProxy, move),
  method(OverlayProxy, position),
  method(OverlayProxy, setPosition),
  method(OverlayProxy, setDefaultCursor),
  {0,0}
};
  
}

#endif // DAGON_OVERLAYPROXY_H_
