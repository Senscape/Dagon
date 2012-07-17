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

#ifndef DG_SPOTPROXY_H
#define DG_SPOTPROXY_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGAudioManager.h"
#include "DGSpot.h"
#include "DGSystem.h"
#include "DGTexture.h"
#include "DGVideoManager.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGSpotProxy : public DGObjectProxy {
public:
    static const char className[];
    static Luna<DGSpotProxy>::RegType methods[];
    
    // Constructor
    DGSpotProxy(lua_State *L) {
        int flags = DGSpotUser;
        float volume = 1.0f;
        
        if (lua_istable(L, 3)) {
            lua_pushnil(L);
            while (lua_next(L, 3) != 0) {
                const char* key = lua_tostring(L, -2);
                
                // We can only read the key as a string, so we have no choice but
                // do an ugly nesting of strcmps()
                if (strcmp(key, "auto") == 0) {
                    if (lua_toboolean(L, -1) == 1) flags = flags | DGSpotAuto;
                    else flags = flags & ~DGSpotAuto;
                }
                
                if (strcmp(key, "loop") == 0) {
                    if (lua_toboolean(L, -1) == 1) flags = flags | DGSpotLoop;
                    else flags = flags & ~DGSpotLoop;
                }
                
                if (strcmp(key, "sync") == 0) {
                    if (lua_toboolean(L, -1) == 1) flags = flags | DGSpotSync;
                    else flags = flags & DGSpotSync;
                }

                if (strcmp(key, "volume") == 0) volume = (float)(lua_tonumber(L, -1) / 100);
                
                lua_pop(L, 1);
            }
        }
        
        if (lua_istable(L, 2)) {
            std::vector<int> arrayOfCoords;
            
            lua_pushnil(L);  // First key
            while (lua_next(L, 2) != 0) {
                // Uses key at index -2 and value at index -1
                arrayOfCoords.push_back(lua_tonumber(L, -1));
                lua_pop(L, 1);
            }
            
            s = new DGSpot(arrayOfCoords, luaL_checknumber(L, 1), flags);
            s->setVolume(volume);
        }
        else luaL_error(L, DGMsg250004);
        
        // Init the base class
        this->setObject(s);
    }
    
    // TODO: In the future we should return a pointer to an attached object
    int attach(lua_State *L) {
        DGAction action;
        DGAudio* audio;
        DGTexture* texture;
        DGVideo* video;
        
        // For the video attach, autoplay defaults to true
        bool autoplay, loop, sync;
        
        int type = (int)luaL_checknumber(L, 1);
        
        switch (type) {
            case AUDIO:
                if (DGCheckProxy(L, 2) == DGObjectAudio) {
                    // Just set the audio object
                    s->setAudio(DGProxyToAudio(L, 2));
                    
                    // Now we get the metatable of the added audio and set it
                    // as a return value
                    lua_getfield(L, LUA_REGISTRYINDEX, DGAudioProxyName);
                    lua_setmetatable(L, -2);
                    
                    return 1;
                }
                else {
                    // If not, create and set (deleted by the Audio Manager)
                    audio = new DGAudio;
                    audio->setResource(luaL_checkstring(L, 2));
                    
                    DGAudioManager::getInstance().registerAudio(audio);
                    
                    if (s->hasFlag(DGSpotLoop))
                        audio->setLoopable(true);
                    
                    s->setAudio(audio);
                }
                
                break;
            case FEED:
                action.type = DGActionFeed;
                action.cursor = DGCursorLook;
                action.luaObject = s->luaObject();                
                strncpy(action.feed, luaL_checkstring(L, 2), DGMaxFeedLength);
                
                if (lua_isstring(L, 3)) {
                    strncpy(action.feedAudio, lua_tostring(L, 3), DGMaxFileLength);
                    action.hasFeedAudio = true;
                }
                else action.hasFeedAudio = false;
                
                s->setAction(&action);
                if (!s->hasColor())
                    s->setColor(0);
                
                break;
            case FUNCTION:
                if (!lua_isfunction(L, 2)) {
                    DGLog::getInstance().error(DGModScript, "%s", DGMsg250006);
                    return 0;
                }
                
                action.type = DGActionFunction;
                action.cursor = DGCursorUse;
                action.luaObject = s->luaObject();
                action.luaHandler = luaL_ref(L, LUA_REGISTRYINDEX); // Pop and return a reference to the table
                
                s->setAction(&action);
                if (!s->hasColor())
                    s->setColor(0);
                
                break;                
            case IMAGE:
                // FIXME: This texture isn't managed. We should be able to communicate with the
                // texture manager, register this texture, and parse its path if necessary, OR
                // use the DGControl register method.
                
                // TODO: Decide here if we have an extension and therefore set the name or the
                // resource of the texture.
                
                texture = new DGTexture;
                texture->setResource(DGConfig::getInstance().path(DGPathRes, luaL_checkstring(L, 2), DGObjectImage));
                
                // If we have a third parameter, use it to set the index inside a bundle
                if (lua_isnumber(L, 3))
                    texture->setIndexInBundle(lua_tonumber(L, 3));
                
                s->setTexture(texture);
                break;
            case SWITCH:
                type = DGCheckProxy(L, 2);
                if (type == DGObjectNode)
                    action.target = DGProxyToNode(L, 2);
                else if (type == DGObjectRoom)
                    action.target = DGProxyToRoom(L, 2);
                else {
                    DGLog::getInstance().error(DGModScript, "%s", DGMsg250005);
                    return 0;
                }
                
                action.type = DGActionSwitch;
                action.cursor = DGCursorForward;
                action.luaObject = s->luaObject();
                
                s->setAction(&action);
                if (!s->hasColor())
                    s->setColor(0);
                
                break;
            case VIDEO:
                if (s->hasFlag(DGSpotAuto)) autoplay = true;
                else autoplay = false;
                
                if (s->hasFlag(DGSpotLoop)) loop = true;
                else loop = false;
                
                if (s->hasFlag(DGSpotSync)) sync = true;
                else sync = false;                
                    
                video = new DGVideo(autoplay, loop, sync);
                
                // TODO: Path is set by the video manager
                video->setResource(DGConfig::getInstance().path(DGPathRes, luaL_checkstring(L, 2), DGObjectVideo));
                s->setVideo(video);
                DGVideoManager::getInstance().registerVideo(video);
                
                break;                
        }

        return 0;
    }
    
    // Set a cursor for the attached action
    int setCursor(lua_State *L) {
        if (s->hasAction()) {
            DGAction* action = s->action();
            
            action->cursor = luaL_checknumber(L, 1);
        }
        
        return 0;
    }
    
    // Check if playing
    int isPlaying(lua_State *L) {
        lua_pushboolean(L, s->isPlaying());
        return 1;
    }    
    
    // Play the spot
    int play(lua_State *L) {
        // WARNING: Potential thread conflict here! Let the manager decide when to pause
        // thread(s).
        
        s->play();
        
        // Test for synced audio or video
        if (s->hasVideo()) {
            if (s->isPlaying() && s->video()->isSynced()) {
                DGControl::getInstance().syncSpot(s);
                return DGScript::getInstance().suspend();
            }
        }
        
        return 0;
    }
    
    // Stop the spot
    int stop(lua_State *L) {
        s->stop();
        return 0;
    }
    
    // Destructor
    ~DGSpotProxy() { delete s; }
    
    DGSpot* ptr() { return s; }
    
private:
    DGSpot* s;  
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char DGSpotProxy::className[] = DGSpotProxyName;

Luna<DGSpotProxy>::RegType DGSpotProxy::methods[] = {
    DGObjectMethods(DGSpotProxy),    
    method(DGSpotProxy, attach),
    method(DGSpotProxy, setCursor),    
    method(DGSpotProxy, isPlaying),    
    method(DGSpotProxy, play),
    method(DGSpotProxy, stop),   
    {0,0}
};

#endif // DG_SPOTPROXY_H
