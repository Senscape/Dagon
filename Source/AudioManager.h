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

#ifndef DG_AUDIOMANAGER_H
#define DG_AUDIOMANAGER_H

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
  
  std::vector<Audio*> _arrayOfAudios;
  std::vector<Audio*> _arrayOfActiveAudios;
  
  bool _isInitialized;
  bool _isRunning;
  
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
  
  // These two methods have similar purposes: clear() notifies the manager
  // that the engine is about to load a new node, which prepares all
  // active audios for release. flush() effectively unloads every audio
  // that is no longer needed.
  void clear();
  void flush();
  
  void init();
  void registerAudio(Audio* target);
  void requestAudio(Audio* target);
  void setOrientation(float* orientation);
  void terminate();
  bool update();
};

#endif // DG_AUDIOMANAGER_H
