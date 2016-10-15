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

#ifndef DAGON_AUDIOMANAGER_H_
#define DAGON_AUDIOMANAGER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Audio.h"
#include "Platform.h"

#ifdef DAGON_MAC
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_thread.h>

#include <set>

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class Config;
class Log;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class AudioManager {
  Config& config;
  Log& log;
  
  ALCdevice* _alDevice;
  ALCcontext* _alContext;
  SDL_mutex* _mutex;
  SDL_Thread* _thread;
  
  std::set<Audio*> _activeAudios;
  
  bool _isInitialized;
  bool _isRunning;
  
  bool _update();
  static int _runThread(void *ptr);
  
  AudioManager();
  AudioManager(AudioManager const&);
  AudioManager& operator=(AudioManager const&);
  ~AudioManager();
  
public:
  static AudioManager& instance() {
    static AudioManager audioManager;
    return audioManager;
  }
  
  void init();
  void registerAudio(Audio* target);
  bool unregisterAudio(Audio* target);
  void setOrientation(float* orientation);
  void terminate();
};
  
}

#endif // DAGON_AUDIOMANAGER_H_
