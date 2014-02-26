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

#ifndef DAGON_NODEPROXY_H_
#define DAGON_NODEPROXY_H_

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by Proxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Control.h"
#include "Node.h"
#include "Room.h"
#include "Script.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define N   "N"
#define E   "E"
#define W   "W"
#define S   "S"
#define U   "U"
#define D   "D"
#define NE  "NE"
#define SE  "SE"
#define NW  "NW"
#define SW  "SW"

namespace dagon {

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class NodeProxy : public ObjectProxy {
public:
  static const char className[];
  static Luna<NodeProxy>::RegType methods[];
  
  // Constructor
  NodeProxy(lua_State *L) {
    n = new Node;
    n->setBundleName(luaL_checkstring(L, 1));
    
    if (lua_gettop(L) == 2)
      n->setDescription(luaL_checkstring(L, 2));
    
    n->setName(luaL_checkstring(L, 1));
    
    // Register the node in the controller (generates the bundle)
    Control::instance().registerObject(n);
    
    if (Script::instance().isExecutingModule()) {
      // A module is being executed, so we know a room
      // object of the same name exists
      
      lua_getglobal(L, Script::instance().module());
      Room* r = ProxyToRoom(L, -1);
      
      // We add the node automatically
      r->addNode(n);
    }
    
    // Init the base class
    this->setObject(n);
  }
  
  // Destructor
  ~NodeProxy() { delete n; }
  
  // Add a spot to the node
  int addSpot(lua_State *L) {
    if (DGCheckProxy(L, 1) == kObjectSpot) {
      n->addSpot(ProxyToSpot(L, 1));
      
      // Now we get the metatable of the added spot and set it
      // as a return value
      lua_getfield(L, LUA_REGISTRYINDEX, SpotProxyName);
      lua_setmetatable(L, -2);
      
      return 1;
    }
    
    return 0;
  }
  
  // Function to link in several directions
  int link(lua_State *L) {
    if (!lua_istable(L, 1)) {
      luaL_error(L, kString14005);
      
      return 0;
    }
    
    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
      const char* key = lua_tostring(L, -2);
      int dir;
      
      // We can only read the key as a string, so we have no choice but
      // do an ugly nesting of strcmps()
      if (strcmp(key, N) == 0) dir = kNorth;
      else if (strcmp(key, W) == 0) dir = kWest;
      else if (strcmp(key, S) == 0) dir = kSouth;
      else if (strcmp(key, E) == 0) dir = kEast;
      else if (strcmp(key, U) == 0) dir = kUp;
      else if (strcmp(key, D) == 0) dir = kDown;
      else if (strcmp(key, NW) == 0) dir = kNorthWest;
      else if (strcmp(key, SW) == 0) dir = kSouthWest;
      else if (strcmp(key, SE) == 0) dir = kSouthEast;
      else if (strcmp(key, NE) == 0) dir = kNorthEast;
      
      if (lua_isfunction(L, -1)) {
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        n->addCustomLink(dir, ref);
        
        // No need to pop the value in this case
      }
      else {
        switch (DGCheckProxy(L, 3)) {
          case kObjectNode:
            n->addLink(dir, (Object*)ProxyToNode(L, 3));
            break;
          case kObjectRoom:
            n->addLink(dir, (Object*)ProxyToRoom(L, 3));
            break;
        }
        
        lua_pop(L, 1);
      }
    }
    
    return 0;
    
  }
  
  // Set an onEnter event
  int onEnter(lua_State *L) {
    int ref = luaL_ref(L, LUA_REGISTRYINDEX); // pop and return a reference to the table.
    n->setEnterEvent(ref);
    
    return 0;
  }
  
  // Set an onEnter event
  int onLeave(lua_State *L) {
    int ref = luaL_ref(L, LUA_REGISTRYINDEX); // pop and return a reference to the table.
    n->setLeaveEvent(ref);
    
    return 0;
  }
  
  // Set a custom footstep
  int setFootstep(lua_State *L) {
    if (DGCheckProxy(L, 1) == kObjectAudio) {
      // Just set the audio object
      n->setFootstep((Audio*)ProxyToAudio(L, 1));
    }
    else {
      // If not, create and set (this is later deleted by the Audio Manager)
      Audio* audio = new Audio;
      audio->setResource(luaL_checkstring(L, 1));
      audio->setVarying(true); // We force variation for footsteps
      
      AudioManager::instance().registerAudio(audio);
      
      n->setFootstep(audio);
    }
    
    return 0;
  }
  
  Node* ptr() { return n; }
  
private:
  Node* n;
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char NodeProxy::className[] = NodeProxyName;

Luna<NodeProxy>::RegType NodeProxy::methods[] = {
  ObjectMethods(NodeProxy),
  method(NodeProxy, addSpot),
  method(NodeProxy, link),
  method(NodeProxy, onEnter),
  method(NodeProxy, onLeave),
  method(NodeProxy, setFootstep),
  {0,0}
};
  
}

#endif // DAGON_NODEPROXY_H_
