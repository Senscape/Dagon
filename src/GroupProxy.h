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

#ifndef DAGON_GROUPPROXY_H_
#define DAGON_GROUPPROXY_H_

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by Proxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Group.h"

namespace dagon {
  
////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class GroupProxy : public ObjectProxy {
 public:
  static const char className[];
  static Luna<GroupProxy>::RegType methods[];

  // Constructor
  GroupProxy(lua_State *L) {
    g = new Group;
    g->setDescription(luaL_checkstring(L, 1));
    
    // Init the base class
    this->setObject(g);
  }

  // Destructor
  ~GroupProxy() { delete g; }

  // Add a spot to the node
  int addSpot(lua_State *L) {
    if (DGCheckProxy(L, 1) == kObjectSpot) {
      g->addSpot(ProxyToSpot(L, 1));
      
      // Now we get the metatable of the added spot and set it
      // as a return value
      lua_getfield(L, LUA_REGISTRYINDEX, SpotProxyName);
      lua_setmetatable(L, -2);
      
      return 1;
    }
    
    return 0;
  }
  
  // Disable the group
  int disable(lua_State *L) {
    g->disable();
    return 0;
  }
  
  // Enable the group
  int enable(lua_State *L) {
    g->enable();
    return 0;
  }
  
  // Play the group
  int play(lua_State *L) {
    g->play();
    return 0;
  }
  
  // Stop the group
  int stop(lua_State *L) {
    g->stop();
    return 0;
  }
  
  Group* ptr() { return g; }
  
 private:
  Group* g;
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char GroupProxy::className[] = GroupProxyName;

Luna<GroupProxy>::RegType GroupProxy::methods[] = {
  ObjectMethods(GroupProxy),
  method(GroupProxy, addSpot),
  method(GroupProxy, disable),
  method(GroupProxy, enable),
  method(GroupProxy, play),
  method(GroupProxy, stop),
  {0,0}
};
  
}

#endif // DAGON_GROUPPROXY_H_
