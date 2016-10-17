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

#ifndef DAGON_AUDIOPROXY_H_
#define DAGON_AUDIOPROXY_H_

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

namespace dagon {

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class AudioProxy : public ObjectProxy {
public:
  static const char className[];
  static Luna<AudioProxy>::RegType methods[];
  
  // Constructor
  AudioProxy(lua_State *L) {
    a = new Audio;
    a->setAudioName(luaL_checkstring(L, 1));
    Control::instance().assetRoom()->claimAsset(a);
    
    if (lua_istable(L, 2)) {
      lua_pushnil(L);
      while (lua_next(L, 2) != 0) {
        const char* key = lua_tostring(L, -2);

        if (strcmp(key, "auto") == 0) {
          a->setAutoplay(lua_toboolean(L, -1));
          a->setStatic();
        }
        if (strcmp(key, "loop") == 0) a->setLoopable(lua_toboolean(L, -1));
        if (strcmp(key, "volume") == 0) a->setDefaultFadeLevel((float)(lua_tonumber(L, -1) / 100));
        if (strcmp(key, "varying") == 0) a->setVarying(lua_toboolean(L, -1));
        
        lua_pop(L, 1);
      }
    }
    
    // Init the base class
    this->setObject(a);
  }
  
  // Destructor
  ~AudioProxy() { delete a; }
  
  // Match with another audio
  int match(lua_State *L) {
    a->match((Audio*)ProxyToAudio(L, 1));
    
    return 0;
  }
  
  // Pause the audio
  int pause(lua_State *L) {
    a->pause();
    
    return 0;
  }
  
  // Play the audio
  int play(lua_State *L) {
    AudioManager::instance().registerAudio(a);
    a->play();
    return 0;
  }
  
  // Set the volume
  int setVolume(lua_State *L) {
    a->forceFadeLevel((float)(lua_tonumber(L, -1) / 100));
    
    return 0;
  }
  
  // Stop the audio
  int stop(lua_State *L) {
    a->stop();
    
    return 0;
  }
  
  // Get the volume
  int volume(lua_State *L) {
    lua_pushnumber(L, a->fadeLevel() * 100);
    
    return 1;
  }
  
  Audio* ptr() { return a; }
  
private:
  Audio* a;
  
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char AudioProxy::className[] = AudioProxyName;

Luna<AudioProxy>::RegType AudioProxy::methods[] = {
  ObjectMethods(AudioProxy),
  method(AudioProxy, match),
  method(AudioProxy, play),
  method(AudioProxy, pause),
  method(AudioProxy, setVolume),
  method(AudioProxy, stop),
  method(AudioProxy, volume),
  {0,0}
};
  
}

#endif // DAGON_AUDIOPROXY_H_
