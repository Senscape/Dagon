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

#ifndef DAGON_VIDEOMANAGER_H_
#define DAGON_VIDEOMANAGER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_thread.h>

#include "Platform.h"
#include "Video.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class Config;
class Log;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class VideoManager {
  Config& config;
  Log& log;
  
  SDL_mutex* _mutex;
  SDL_Thread* _thread;
  std::vector<Video*> _arrayOfVideos;
  std::vector<Video*> _arrayOfActiveVideos;
  
  bool _isInitialized;
  bool _isRunning;
  
  static int _runThread(void *ptr);
  
  VideoManager();
  VideoManager(VideoManager const&);
  VideoManager& operator=(VideoManager const&);
  ~VideoManager();
  
public:
  static VideoManager& instance() {
    static VideoManager videoManager;
    return videoManager;
  }
  
  void init();
  void flush();
  void registerVideo(Video* target);
  void requestVideo(Video* target);
  void terminate();
  bool update();
};
  
}

#endif // DAGON_VIDEOMANAGER_H_
