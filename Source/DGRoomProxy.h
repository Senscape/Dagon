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

#ifndef DG_ROOMPROXY_H
#define DG_ROOMPROXY_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGAudioManager.h"
#include "DGControl.h"
#include "DGRoom.h"
#include "DGTimerManager.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGRoomProxy : public ObjectProxy {
public:
    static const char className[];
    static Luna<DGRoomProxy>::RegType methods[];
    
    // Constructor
    DGRoomProxy(lua_State *L) {
        // TODO: Support custom room names with a second parameter
        
        r = new DGRoom;
        r->setName(luaL_checkstring(L, 1));
        
        // Register the new room
        DGControl::instance().registerObject(r);
        
        // Init the base class
        this->setObject(r);
    }
    
    // Destructor
    ~DGRoomProxy() { delete r; }
    
    // Add an audio to the room
    int addAudio(lua_State *L) {
        if (DGCheckProxy(L, 1) == kObjectAudio) {
            r->addAudio(DGProxyToAudio(L, 1));
            
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
            r->addNode(DGProxyToNode(L, 1));
            
            // Now we get the metatable of the added node and set it
            // as a return value
            lua_getfield(L, LUA_REGISTRYINDEX, NodeProxyName);
            lua_setmetatable(L, -2);
            
            return 1;
        }
        
        return 0;
    }
    
    // Set the default footstep
    int setDefaultFootstep(lua_State *L) {
        if (DGCheckProxy(L, 1) == kObjectAudio) {
            // Just set the audio object
             r->setDefaultFootstep((Audio*)DGProxyToAudio(L, 1));
        }
        else {
            // If not, create and set (this is later deleted by the Audio Manager)
            Audio* audio = new Audio;
            audio->setResource(luaL_checkstring(L, 1));
          
            DGAudioManager::instance().registerAudio(audio);
            
            r->setDefaultFootstep(audio);
        }
        
        return 0;
    }
    
    int startTimer(lua_State *L) {
        if (!lua_isfunction(L, -1)) {
            Log::instance().trace(kModScript, "%s", kString14009);
            
            return 0;
        }
        
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);  // Pop and return a reference to the table.
        bool shouldLoop = lua_toboolean(L, 2);
        int handle = DGTimerManager::instance().create(luaL_checknumber(L, 1), shouldLoop, ref, r->luaObject());
        
        lua_pushnumber(L, handle);
        
        return 1;
    }
    
    DGRoom* ptr() { return r; }
    
private:
    DGRoom* r;

};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char DGRoomProxy::className[] = DGRoomProxyName;

Luna<DGRoomProxy>::RegType DGRoomProxy::methods[] = {
    ObjectMethods(DGRoomProxy),    
    method(DGRoomProxy, addAudio),
    method(DGRoomProxy, addNode),
    method(DGRoomProxy, setDefaultFootstep),    
    method(DGRoomProxy, startTimer),     
    {0,0}
};

#endif // DG_ROOMPROXY_H
