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

#ifndef DG_PROXY_H
#define DG_PROXY_H

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
#include "Image.h"
#include "Node.h"
#include "Overlay.h"
#include "DGRoom.h"
#include "DGSpot.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define AudioProxyName "Audio"
#define ButtonProxyName "Button"
#define ImageProxyName "Image"
#define NodeProxyName "Node"
#define OverlayProxyName "Overlay"
#define DGRoomProxyName "Room"
#define DGSlideProxyName "Slide"
#define DGSpotProxyName "Spot"

#define method(class, name) {#name, &class::name}

static int DGCheckProxy(lua_State *L, int idx); // Returns the object type
static Audio* DGProxyToAudio(lua_State *L, int idx);
static Button* DGProxyToButton(lua_State *L, int idx);
static Image* DGProxyToImage(lua_State *L, int idx);
static Node* DGProxyToNode(lua_State *L, int idx);
static Overlay* DGProxyToOverlay(lua_State *L, int idx);
static DGRoom* DGProxyToRoom(lua_State *L, int idx);
static Node* DGProxyToSlide(lua_State *L, int idx);
static DGSpot* DGProxyToSpot(lua_State *L, int idx);

// Now that the proxy functions has been declared, we
// proceed to include the remaining headers

#include "ObjectProxy.h"

#include "AudioProxy.h"
#include "ButtonProxy.h"
#include "ImageProxy.h"
#include "NodeProxy.h"
#include "OverlayProxy.h"
#include "DGRoomProxy.h"
#include "DGSlideProxy.h"
#include "DGSpotProxy.h"

// We include non-proxy libraries here as well

#include "DGCamLib.h"
#include "ConfigLib.h"
#include "DGEffectsLib.h"
#include "DGCursorLib.h"
#include "DGSystemLib.h"

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
        
        if (_checkutype(L, idx, ImageProxy::className))
            return kObjectImage; // It's an image        
        
        if (_checkutype(L, idx, NodeProxy::className))
            return kObjectNode; // It's a node
        
        if (_checkutype(L, idx, OverlayProxy::className))
            return kObjectOverlay; // It's an overlay        
        
        if (_checkutype(L, idx, DGRoomProxy::className))
            return kObjectRoom; // It's a room
        
        if (_checkutype(L, idx, DGSlideProxy::className))
            return kObjectSlide; // It's a slide
        
        if (_checkutype(L, idx, DGSpotProxy::className))
            return kObjectSpot; // It's a spot
        
        // No, it's Superman!
        
        return kObjectGeneric; // None of the above, we return a generic type
    }
    
    return kObjectNone;
}

Audio* DGProxyToAudio(lua_State *L, int idx) {
    AudioProxy* a = Luna<AudioProxy>::check(L, idx);
    return a->ptr();
}

Button* DGProxyToButton(lua_State *L, int idx) {
    ButtonProxy* b = Luna<ButtonProxy>::check(L, idx);
    return b->ptr();
}

Image* DGProxyToImage(lua_State *L, int idx) {
    ImageProxy* i = Luna<ImageProxy>::check(L, idx);
    return i->ptr();
}

Node* DGProxyToNode(lua_State *L, int idx) {
    NodeProxy* n = Luna<NodeProxy>::check(L, idx);
    return n->ptr();
}

Overlay* DGProxyToOverlay(lua_State *L, int idx) {
    OverlayProxy* o = Luna<OverlayProxy>::check(L, idx);
    return o->ptr();    
}

DGRoom* DGProxyToRoom(lua_State *L, int idx) {
    DGRoomProxy* r = Luna<DGRoomProxy>::check(L, idx);
    return r->ptr();    
}

Node* DGProxyToSlide(lua_State *L, int idx) {
    DGSlideProxy* s = Luna<DGSlideProxy>::check(L, idx);
    return s->ptr();
}

DGSpot* DGProxyToSpot(lua_State *L, int idx) {
    DGSpotProxy* s = Luna<DGSpotProxy>::check(L, idx);
    return s->ptr();    
}

#endif // DG_PROXY_H
