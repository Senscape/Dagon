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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <SDL2/SDL_timer.h>

#include "Config.h"
#include "Log.h"
#include "VideoManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

VideoManager::VideoManager() :
config(Config::instance()),
log(Log::instance())
{
  _isInitialized = false;
  _isRunning = false;
  _mutex = SDL_CreateMutex();
  if (!_mutex)
    log.error(kModVideo, "%s", kString18001);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

VideoManager::~VideoManager() {
  SDL_DestroyMutex(_mutex);
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void VideoManager::flush() {
  bool done = false;
  if (_isInitialized) {
    if (!_arrayOfActiveVideos.empty()) {
      vector<Video*>::iterator it;
      while (!done) {
        if (SDL_LockMutex(_mutex) == 0) {
          it = _arrayOfActiveVideos.begin();
          done = true;
          while (it != _arrayOfActiveVideos.end()) {
            if ((*it)->retainCount() == 0) {
              (*it)->unload();
              _arrayOfActiveVideos.erase(it);
              done = false;
              break;
            }
            else it++;
          }
          SDL_UnlockMutex(_mutex);
        } else {
          log.error(kModVideo, "%s", kString18002);
        }
      }
    }
  }
}

void VideoManager::init() {
  log.trace(kModVideo, "%s", kString17001);
  log.info(kModVideo, "%s: %s", kString17006, theora_version_string());
  
  // Eventually lots of Theora initialization process will be moved here
  
  _isInitialized = true;
  _isRunning = true;
  
  _thread = SDL_CreateThread(_runThread, "VideoManager", (void*)NULL);
  if (!_thread) {
    log.error(kModVideo, "%s:%s", kString18003, SDL_GetError());
  }
}

void VideoManager::registerVideo(Video* target) {
  _arrayOfVideos.push_back(target);
}

void VideoManager::requestVideo(Video* target) {
  if (!target->isLoaded()) {
    target->load();
  }
  
  // If the audio is not active, then it's added to
  // that vector
  
  bool isActive = false;
  std::vector<Video*>::iterator it;
  it = _arrayOfActiveVideos.begin();
  
  while (it != _arrayOfActiveVideos.end()) {
    if ((*it) == target) {
      isActive = true;
      break;
    }
    
    it++;
  }
  
  if (!isActive) {
    if (SDL_LockMutex(_mutex) == 0) {
      _arrayOfActiveVideos.push_back(target);
      SDL_UnlockMutex(_mutex);
    } else {
      log.error(kModVideo, "%s", kString18002);
    }
  }
}

void VideoManager::terminate() {
  _isRunning = false;
  
  int threadReturnValue;
  SDL_WaitThread(_thread, &threadReturnValue);
  
  // WARNING: This code assumes videos are never created
  // directly in the script
  if (!_arrayOfVideos.empty()) {
    if (SDL_LockMutex(_mutex) == 0) {
      vector<Video*>::iterator it = _arrayOfVideos.begin();
      while (it != _arrayOfVideos.end()) {
        delete *it;
        it++;
      }
      SDL_UnlockMutex(_mutex);
    } else {
      log.error(kModVideo, "%s", kString18002);
    }
  }
}

bool VideoManager::update() {
  if (_isRunning) {
    if (!_arrayOfActiveVideos.empty()) {
      if (SDL_LockMutex(_mutex) == 0) {
        vector<Video*>::iterator it = _arrayOfActiveVideos.begin();
        while (it != _arrayOfActiveVideos.end()) {
          (*it)->update();
          it++;
        }
        SDL_UnlockMutex(_mutex);
      } else {
        log.error(kModVideo, "%s", kString18002);
      }
    }
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

int VideoManager::_runThread(void *ptr) {
  while (VideoManager::instance().update()) {
    SDL_Delay(1);
  }
  return 0;
}
