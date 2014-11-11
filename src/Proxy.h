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

#ifndef DAGON_PROXY_H_
#define DAGON_PROXY_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}
#include "Luna.h"

#include "Platform.h"
#include "Audio.h"
#include "Button.h"
#include "Group.h"
#include "Image.h"
#include "Node.h"
#include "Overlay.h"
#include "Room.h"
#include "Spot.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define AudioProxyName "Audio"
#define ButtonProxyName "Button"
#define GroupProxyName "Group"
#define ImageProxyName "Image"
#define NodeProxyName "Node"
#define OverlayProxyName "Overlay"
#define RoomProxyName "Room"
#define SlideProxyName "Slide"
#define SpotProxyName "Spot"

#define method(class, name) {#name, &class::name}

// We include non-proxy libraries here as well

#include "CameraLib.h"
#include "ConfigLib.h"
#include "EffectsLib.h"
#include "CursorLib.h"
#include "SystemLib.h"

namespace dagon {
  
static int DGCheckProxy(lua_State *L, int idx); // Returns the object type
static Audio* ProxyToAudio(lua_State *L, int idx);
static Button* ProxyToButton(lua_State *L, int idx);
static Group* ProxyToGroup(lua_State *L, int idx);
static Image* ProxyToImage(lua_State *L, int idx);
static Node* ProxyToNode(lua_State *L, int idx);
static Overlay* ProxyToOverlay(lua_State *L, int idx);
static Room* ProxyToRoom(lua_State *L, int idx);
static Node* ProxyToSlide(lua_State *L, int idx);
static Spot* ProxyToSpot(lua_State *L, int idx);
  
}

// Now that the proxy functions has been declared, we
// proceed to include the remaining headers
  
#include "ObjectProxy.h"
  
#include "AudioProxy.h"
#include "ButtonProxy.h"
#include "GroupProxy.h"
#include "ImageProxy.h"
#include "NodeProxy.h"
#include "OverlayProxy.h"
#include "RoomProxy.h"
#include "SlideProxy.h"
#include "SpotProxy.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

// NOTE: This isn't so efficient because the operations required
// to check the type of object are essentially the same as those
// required to perform the cast (via Luna). Since we don't do this
// very often, it will do for now.
//
// In the future, we should find some way to safely try a cast
// without triggering an assertion in Lua.

// Custom check user type function that simply returns true if the
// object in the given index is the desired one
static bool _checkutype(lua_State *L, int idx, const char* name) {
  int v1, v2;
  
  lua_getfield(L, LUA_REGISTRYINDEX, name);  // Get correct metatable
  v1 = lua_getmetatable(L, idx);
  v2 = lua_rawequal(L, -1, -2);
  lua_pop(L, 2);  // Remove both metatables
  
  if (!v1 || !v2)
    return false; // Not the type
  else
    return true; // Correct type
}

int DGCheckProxy(lua_State *L, int idx) {
  // Let's make sure this is a userdata first
  if (lua_isuserdata(L, idx)) {
    // Good, now check against each user type
    
    if (_checkutype(L, idx, AudioProxy::className))
      return kObjectAudio; // It's an audio
    
    if (_checkutype(L, idx, ButtonProxy::className))
      return kObjectButton; // It's a button
    
    if (_checkutype(L, idx, GroupProxy::className))
      return kObjectGroup; // It's an group
    
    if (_checkutype(L, idx, ImageProxy::className))
      return kObjectImage; // It's an image
    
    if (_checkutype(L, idx, NodeProxy::className))
      return kObjectNode; // It's a node
    
    if (_checkutype(L, idx, OverlayProxy::className))
      return kObjectOverlay; // It's an overlay
    
    if (_checkutype(L, idx, RoomProxy::className))
      return kObjectRoom; // It's a room
    
    if (_checkutype(L, idx, SlideProxy::className))
      return kObjectSlide; // It's a slide
    
    if (_checkutype(L, idx, SpotProxy::className))
      return kObjectSpot; // It's a spot
    
    // No, it's Superman!
    
    return kObjectGeneric; // None of the above, we return a generic type
  }
  
  return kObjectNone;
}

Audio* ProxyToAudio(lua_State *L, int idx) {
  AudioProxy* a = Luna<AudioProxy>::check(L, idx);
  return a->ptr();
}

Button* ProxyToButton(lua_State *L, int idx) {
  ButtonProxy* b = Luna<ButtonProxy>::check(L, idx);
  return b->ptr();
}
  
Group* ProxyToGroup(lua_State *L, int idx) {
  GroupProxy* g = Luna<GroupProxy>::check(L, idx);
  return g->ptr();
}

Image* ProxyToImage(lua_State *L, int idx) {
  ImageProxy* i = Luna<ImageProxy>::check(L, idx);
  return i->ptr();
}

Node* ProxyToNode(lua_State *L, int idx) {
  NodeProxy* n = Luna<NodeProxy>::check(L, idx);
  return n->ptr();
}

Overlay* ProxyToOverlay(lua_State *L, int idx) {
  OverlayProxy* o = Luna<OverlayProxy>::check(L, idx);
  return o->ptr();
}

Room* ProxyToRoom(lua_State *L, int idx) {
  RoomProxy* r = Luna<RoomProxy>::check(L, idx);
  return r->ptr();
}

Node* ProxyToSlide(lua_State *L, int idx) {
  SlideProxy* s = Luna<SlideProxy>::check(L, idx);
  return s->ptr();
}

Spot* ProxyToSpot(lua_State *L, int idx) {
  SpotProxy* s = Luna<SpotProxy>::check(L, idx);
  return s->ptr();
}
  
}

#endif // DAGON_PROXY_H_
