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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <SDL2/SDL_timer.h>

#include "Script.h"
#include "System.h"
#include "TimerManager.h"
#include "Language.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

TimerManager::TimerManager() {
  _handles = 0;
  _luaObject = 0;
  _mutex = SDL_CreateMutex();
  if (!_mutex)
    return;
  _isRunning = true;
  
  _thread = SDL_CreateThread(_runThread, "TimerManager", (void*)NULL);
  if (!_thread) {
    _isRunning = false;
  }
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

TimerManager::~TimerManager() {
  SDL_DestroyMutex(_mutex);
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

std::vector<DGTimer>& TimerManager::timers() {
  return _arrayOfTimers;
}

double TimerManager::timeElapsed(const DGTimer &timer) {
  if (SDL_LockMutex(_mutex) == 0) {
    double currentTime = SDL_GetTicks() / 1000;
    double elapsed = currentTime - timer.lastTime;
    SDL_UnlockMutex(_mutex);
    return elapsed;
  }

  return 0;
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

bool TimerManager::checkManual(int handle, int precision) {
  DGTimer* timer = _lookUp(handle);
  
  if (timer->isEnabled) {
    double currentTime = SDL_GetTicks() / precision;
    double duration = currentTime - timer->lastTime;
    
    if (duration > timer->trigger) {
      timer->lastTime = currentTime;
      return true;
    }
  }
  
  return false;
}

int TimerManager::create(double trigger, bool shouldLoop, int handlerForLua, int luaObject) {
  DGTimer timer;
  
  timer.handle = _handles;
  timer.hasTriggered = false;
  timer.isEnabled = true;
  timer.isLoopable = shouldLoop;
  timer.lastTime = SDL_GetTicks() / 1000;
  timer.type = DGTimerNormal;
  
  timer.trigger = trigger;
  timer.luaHandler = handlerForLua;
  timer.luaObject = luaObject;
  
  if (SDL_LockMutex(_mutex) == 0) {
    _arrayOfTimers.push_back(timer);
    SDL_UnlockMutex(_mutex);
  }
  
  _handles++;
  
  return timer.handle;
}

int TimerManager::createInternal(double trigger, void (*callback)()) {
  DGTimer timer;
  
  timer.handle = _handles;
  timer.hasTriggered = false;
  timer.handler = callback;
  timer.isEnabled = true;
  timer.isLoopable = false;
  timer.lastTime = SDL_GetTicks() / 1000;
  timer.type = DGTimerInternal;
  
  timer.trigger = trigger;
  
  if (SDL_LockMutex(_mutex) == 0) {
    _arrayOfTimers.push_back(timer);
    SDL_UnlockMutex(_mutex);
  }
  
  _handles++;
  
  return timer.handle;
}

int TimerManager::createManual(double trigger, int precision) {
  DGTimer timer;
  
  timer.handle = _handles;
  timer.hasTriggered = false;
  timer.isEnabled = true;
  timer.isLoopable = false;
  timer.lastTime = SDL_GetTicks() / precision;
  timer.type = DGTimerManual;
  
  timer.trigger = trigger;
  
  if (SDL_LockMutex(_mutex) == 0) {
    _arrayOfTimers.push_back(timer);
    SDL_UnlockMutex(_mutex);
  }
  
  _handles++;
  
  return timer.handle;
}

void TimerManager::destroy(int handle) {
  // Must implement
}

void TimerManager::disable(int handle) {
  DGTimer* timer = _lookUp(handle);
  timer->isEnabled = false;
}

void TimerManager::enable(int handle) {
  DGTimer* timer = _lookUp(handle);
  timer->isEnabled = true;
  timer->lastTime = SDL_GetTicks() / 1000;
}

void TimerManager::process() {
  if (SDL_LockMutex(_mutex) == 0) {
    bool mutexUnlocked = false;
    std::vector<DGTimer>::iterator it = _arrayOfTimers.begin();
    while (it != _arrayOfTimers.end()) {
      if ((*it).isEnabled && ((*it).type != DGTimerManual)) {
        if ((*it).hasTriggered) {
          bool keepProcessing = true;
          switch ((*it).type) {
            case DGTimerInternal:
              (*it).handler();
              keepProcessing = false;
              break;
              
            case DGTimerNormal:
              if ((*it).luaObject) { // Belongs to a Lua object?
                if ((*it).luaObject != _luaObject) {
                  (*it).hasTriggered = false;
                  (*it).lastTime = SDL_GetTicks() / 1000; // Reset timer
                  break; // Do not invoke the handler
                }
              }
              
              if ((*it).isLoopable) {
                (*it).hasTriggered = false;
                (*it).lastTime = SDL_GetTicks() / 1000;
              }
              else {
                // Should destroy in reality
                (*it).isEnabled = false;
              }
              
              // Must unlock here for precaution
              SDL_UnlockMutex(_mutex);
              mutexUnlocked = true;
              Script::instance().processCallback((*it).luaHandler, 0);
              keepProcessing = false;
              break;
          }
          
          if (!keepProcessing)
            break; // In case the vector has changed, break the loop
        }
      }
      ++it;
    }
    if (!mutexUnlocked) SDL_UnlockMutex(_mutex);
  }
}

void TimerManager::setLuaObject(int luaObject) {
  _luaObject = luaObject;
}

void TimerManager::setSystem(System* theSystem) {
  system = theSystem;
}

void TimerManager::terminate() {
  _isRunning = false;
  
  int threadReturnValue;
  SDL_WaitThread(_thread, &threadReturnValue);
}

// FIXME: This is quite sucky. Timers keep looping and being checked even if they
// were already triggered. Should have different arrays here per each kind of timer.
bool TimerManager::update() {
  if (_isRunning) {
    if (SDL_LockMutex(_mutex) == 0) {
      std::vector<DGTimer>::iterator it = _arrayOfTimers.begin();
      while (it != _arrayOfTimers.end()) {
        double currentTime = SDL_GetTicks() / 1000;
        double duration = currentTime - (*it).lastTime;
        DGTimer* timer = &(*it);
        if (timer->isEnabled && (timer->type != DGTimerManual)) {
          if ((duration > timer->trigger) && !timer->hasTriggered) {
            timer->hasTriggered = true;
          }
        }
        ++it;
      }
      SDL_UnlockMutex(_mutex);
    }
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

DGTimer* TimerManager::_lookUp(int handle) {
  if (SDL_LockMutex(_mutex) == 0) {
    std::vector<DGTimer>::iterator it = _arrayOfTimers.begin();
    while (it != _arrayOfTimers.end()) {
      if ((*it).handle == handle) {
        SDL_UnlockMutex(_mutex);
        return &(*it);
      }
      ++it;
    }
    SDL_UnlockMutex(_mutex);
  }
  return NULL;
}

int TimerManager::_runThread(void *ptr) {
  while (TimerManager::instance().update()) {
    SDL_Delay(1);
  }
  return 0;
}

}
