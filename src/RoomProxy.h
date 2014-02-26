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

#ifndef DAGON_ROOMPROXY_H_
#define DAGON_ROOMPROXY_H_

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by Proxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "AudioManager.h"
#include "Control.h"
#include "Room.h"
#include "TimerManager.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class RoomProxy : public ObjectProxy {
public:
  static const char className[];
  static Luna<RoomProxy>::RegType methods[];
  
  // Constructor
  RoomProxy(lua_State *L) {
    // TODO: Support custom room names with a second parameter
    
    r = new Room;
    r->setName(luaL_checkstring(L, 1));
    
    // Register the new room
    Control::instance().registerObject(r);
    
    // Init the base class
    this->setObject(r);
  }
  
  // Destructor
  ~RoomProxy() { delete r; }
  
  // Add an audio to the room
  int addAudio(lua_State *L) {
    if (DGCheckProxy(L, 1) == kObjectAudio) {
      r->addAudio(ProxyToAudio(L, 1));
      
      // Now we get the metatable of the added audio and set it
      // as a return value
      lua_getfield(L, LUA_REGISTRYINDEX, AudioProxyName);
      lua_setmetatable(L, -2);
      
      return 1;
    }
    
    return 0;
  }
  
  // Add a node to the room
  int addNode(lua_State *L) {
    if (DGCheckProxy(L, 1) == kObjectNode) {
      r->addNode(ProxyToNode(L, 1));
      
      // Now we get the metatable of the added node and set it
      // as a return value
      lua_getfield(L, LUA_REGISTRYINDEX, NodeProxyName);
      lua_setmetatable(L, -2);
      
      return 1;
    }
    
    return 0;
  }
  
  // Add a slide to the room
  int addSlide(lua_State *L) {
    if (DGCheckProxy(L, 1) == kObjectSlide) {
      r->addNode(ProxyToSlide(L, 1));
      
      // Now we get the metatable of the added node and set it
      // as a return value
      lua_getfield(L, LUA_REGISTRYINDEX, SlideProxyName);
      lua_setmetatable(L, -2);
      
      return 1;
    }
    
    return 0;
  }
  
  // Set an onEnter event
  int onEnter(lua_State *L) {
    int ref = luaL_ref(L, LUA_REGISTRYINDEX); // pop and return a reference to the table.
    r->setEnterEvent(ref);
    
    return 0;
  }
  
  // Set the default footstep
  int setDefaultFootstep(lua_State *L) {
    if (DGCheckProxy(L, 1) == kObjectAudio) {
      // Just set the audio object
      r->setDefaultFootstep((Audio*)ProxyToAudio(L, 1));
    }
    else {
      // If not, create and set (this is later deleted by the Audio Manager)
      Audio* audio = new Audio;
      audio->setResource(luaL_checkstring(L, 1));
      audio->setVarying(true); // We force variation for footsteps
      
      AudioManager::instance().registerAudio(audio);
      
      r->setDefaultFootstep(audio);
    }
    
    return 0;
  }
  
  int setEffects(lua_State *L) {
    if (!lua_istable(L, 1)) {
      luaL_error(L, kString14013);
      return 0;
    }
    
    SettingCollection theEffects;
    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
      const char* key = lua_tostring(L, -2);
      if (strcmp(key, "dustColor") == 0) {
        theEffects[key].value = static_cast<uint32_t>(lua_tonumber(L, 3));
      }
      else {
        theEffects[key].value = static_cast<int>(lua_tonumber(L, 3));
      }
      lua_pop(L, 1);
    }
    r->setEffects(theEffects);
    return 0;
  }
  
  int startTimer(lua_State *L) {
    if (!lua_isfunction(L, -1)) {
      Log::instance().trace(kModScript, "%s", kString14009);
      
      return 0;
    }
    
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);  // Pop and return a reference to the table.
    bool shouldLoop = lua_toboolean(L, 2);
    int handle = TimerManager::instance().create(luaL_checknumber(L, 1), shouldLoop, ref, r->luaObject());
    
    lua_pushnumber(L, handle);
    
    return 1;
  }
  
  Room* ptr() { return r; }
  
private:
  Room* r;
  
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char RoomProxy::className[] = RoomProxyName;

Luna<RoomProxy>::RegType RoomProxy::methods[] = {
  ObjectMethods(RoomProxy),
  method(RoomProxy, addAudio),
  method(RoomProxy, addNode),
  method(RoomProxy, addSlide),
  method(RoomProxy, onEnter),
  method(RoomProxy, setDefaultFootstep),
  method(RoomProxy, setEffects),
  method(RoomProxy, startTimer),
  {0,0}
};
  
}

#endif // DAGON_ROOMPROXY_H_
