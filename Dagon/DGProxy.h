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

#include "DGPlatform.h"
#include "DGAudio.h"
#include "DGButton.h"
#include "DGImage.h"
#include "DGNode.h"
#include "DGOverlay.h"
#include "DGRoom.h"
#include "DGSpot.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define DGAudioProxyName "Audio"
#define DGButtonProxyName "Button"
#define DGImageProxyName "Image"
#define DGNodeProxyName "Node"
#define DGOverlayProxyName "Overlay"
#define DGRoomProxyName "Room"
#define DGSpotProxyName "Spot"

#define method(class, name) {#name, &class::name}

static int DGCheckProxy(lua_State *L, int idx); // Returns the object type
static DGAudio* DGProxyToAudio(lua_State *L, int idx);
static DGButton* DGProxyToButton(lua_State *L, int idx);
static DGImage* DGProxyToImage(lua_State *L, int idx);
static DGNode* DGProxyToNode(lua_State *L, int idx);
static DGOverlay* DGProxyToOverlay(lua_State *L, int idx);
static DGRoom* DGProxyToRoom(lua_State *L, int idx);
static DGSpot* DGProxyToSpot(lua_State *L, int idx);

// Now that the proxy functions has been declared, we
// proceed to include the remaining headers

#include "DGObjectProxy.h"

#include "DGAudioProxy.h"
#include "DGButtonProxy.h"
#include "DGImageProxy.h"
#include "DGNodeProxy.h"
#include "DGOverlayProxy.h"
#include "DGRoomProxy.h"
#include "DGSpotProxy.h"

// We include non-proxy libraries here as well

#include "DGCamLib.h"
#include "DGConfigLib.h"
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

        if (_checkutype(L, idx, DGAudioProxy::className))
            return DGObjectAudio; // It's an audio
        
        if (_checkutype(L, idx, DGButtonProxy::className))
            return DGObjectButton; // It's a button          
        
        if (_checkutype(L, idx, DGImageProxy::className))
            return DGObjectImage; // It's an image        
        
        if (_checkutype(L, idx, DGNodeProxy::className))
            return DGObjectNode; // It's a node
        
        if (_checkutype(L, idx, DGOverlayProxy::className))
            return DGObjectOverlay; // It's an overlay        
        
        if (_checkutype(L, idx, DGRoomProxy::className))
            return DGObjectRoom; // It's a room
        
        if (_checkutype(L, idx, DGSpotProxy::className))
            return DGObjectSpot; // It's a room        
        
        return DGObjectGeneric; // None of the above, we return a generic type
    }
    
    return DGObjectNone;
}

DGAudio* DGProxyToAudio(lua_State *L, int idx) {
    DGAudioProxy* a = Luna<DGAudioProxy>::check(L, idx);
    return a->ptr();
}

DGButton* DGProxyToButton(lua_State *L, int idx) {
    DGButtonProxy* b = Luna<DGButtonProxy>::check(L, idx);
    return b->ptr();
}

DGImage* DGProxyToImage(lua_State *L, int idx) {
    DGImageProxy* i = Luna<DGImageProxy>::check(L, idx);
    return i->ptr();
}

DGNode* DGProxyToNode(lua_State *L, int idx) {
    DGNodeProxy* n = Luna<DGNodeProxy>::check(L, idx);
    return n->ptr();
}

DGOverlay* DGProxyToOverlay(lua_State *L, int idx) {
    DGOverlayProxy* o = Luna<DGOverlayProxy>::check(L, idx);
    return o->ptr();    
}

DGRoom* DGProxyToRoom(lua_State *L, int idx) {
    DGRoomProxy* r = Luna<DGRoomProxy>::check(L, idx);
    return r->ptr();    
}

DGSpot* DGProxyToSpot(lua_State *L, int idx) {
    DGSpotProxy* s = Luna<DGSpotProxy>::check(L, idx);
    return s->ptr();    
}

#endif // DG_PROXY_H
