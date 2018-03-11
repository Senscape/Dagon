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

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <SDL2/SDL_thread.h>

#include "Colors.h"
#include "Config.h"
#include "Log.h"
#include "Object.h"

namespace dagon {

static SDL_mutex* _lock;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Log::Log() :
config(Config::instance()) {
  _lock = SDL_CreateMutex();
  assert(!!_lock);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

Log::~Log() {
  if (_filestr.is_open())
    _filestr.close();
  SDL_DestroyMutex(_lock);
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void Log::command(int forModule, const char* theString, ...) {
  char buffer[kMaxLogLength];
  va_list args;
  
  va_start(args, theString);
  vsnprintf(&buffer[0], kMaxLogLength, theString, args);
  va_end(args);
  
  LogData data = _makeLogData(kColorBrightMagenta, forModule, kLogTrace);
  data.line = buffer;
  _log(&data);
}

void Log::error(int forModule, const char* theString, ...) {
  char buffer[kMaxLogLength];
  va_list args;
  
  va_start(args, theString);
  vsnprintf(&buffer[0], kMaxLogLength, theString, args);
  va_end(args);
  
  LogData data = _makeLogData(kColorBrightRed, forModule, kLogError);
  data.line = buffer;
  _log(&data);
}

void Log::info(int forModule, const char* theString, ...) {
  char buffer[kMaxLogLength];
  va_list args;
  
  va_start(args, theString);
  vsnprintf(&buffer[0], kMaxLogLength, theString, args);
  va_end(args);
  
  LogData data = _makeLogData(kColorBrightCyan, forModule, kLogTrace);
  data.line = buffer;
  _log(&data);
}

void Log::trace(int forModule, const char* theString, ...) {
  char buffer[kMaxLogLength];
  va_list args;
  
  va_start(args, theString);
  vsnprintf(&buffer[0], kMaxLogLength, theString, args);
  va_end(args);
  
  LogData data = _makeLogData(kColorWhite, forModule, kLogTrace);
  data.line = buffer;
  _log(&data);
}

void Log::warning(int forModule, const char* theString, ...) {
  char buffer[kMaxLogLength];
  va_list args;
  
  va_start(args, theString);
  vsnprintf(&buffer[0], kMaxLogLength, theString, args);
  va_end(args);
  
  LogData data = _makeLogData(kColorYellow, forModule, kLogWarning);
  data.line = buffer;
  _log(&data);
}

////////////////////////////////////////////////////////////
// Implementation - Console helpers
////////////////////////////////////////////////////////////

bool Log::beginIteratingHistory() {
  if (SDL_LockMutex(_lock) != 0) {
    assert(false);
    return false;
  }

  if (!_history.empty()) {
    _it = _history.rbegin();
    SDL_UnlockMutex(_lock);
    return true;
  }

  SDL_UnlockMutex(_lock);
  return false;
}

bool Log::iterateHistory() {
  if (SDL_LockMutex(_lock) != 0) {
    assert(false);
    return false;
  }

  ++_it;
  if (_it == (_history.rend() - 1) || _history.empty()) {
    SDL_UnlockMutex(_lock);
    return false; // Bypass the first line
  } else {
    SDL_UnlockMutex(_lock);
    return true;
  }
}

void Log::getCurrentLine(LogData* pointerToLogData) {
  if (SDL_LockMutex(_lock) != 0)
    assert(false);

  assert(_it != _history.rend());
  pointerToLogData->line = _it->line;
  pointerToLogData->color = _it->color;
  pointerToLogData->module = _it->module;
  pointerToLogData->type = _it->type;
  SDL_UnlockMutex(_lock);
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void Log::_log(LogData* data) {
  if (SDL_LockMutex(_lock) != 0)
    assert(false);

  if (config.log) {
    if (!_filestr.is_open())
      _filestr.open(config.path(kPathUserData, kDefLogFile,
                                kObjectGeneric).c_str(),
                    std::ofstream::app);
    
    time_t now = time(0);
    struct tm* tm = localtime(&now);
    
    _filestr << "[";
    
    switch(tm->tm_mon+1) {
      case 1:   _filestr << "Jan "; break;
      case 2:   _filestr << "Feb "; break;
      case 3:   _filestr << "Mar "; break;
      case 4:   _filestr << "Apr "; break;
      case 5:   _filestr << "May "; break;
      case 6:   _filestr << "Jun "; break;
      case 7:   _filestr << "Jul "; break;
      case 8:   _filestr << "Aug "; break;
      case 9:   _filestr << "Sep "; break;
      case 10:  _filestr << "Oct "; break;
      case 11:  _filestr << "Nov "; break;
      case 12:  _filestr << "Dec "; break;
      default: {
        assert(false);
      }
    }
    
    if (tm->tm_mday < 10)
      _filestr << " ";
    
    _filestr << tm->tm_mday << " " << tm->tm_hour << ":";
    
    if (tm->tm_min < 10)
      _filestr << "0";
    
    _filestr << tm->tm_min << ":";
    
    if (tm->tm_sec < 10)
      _filestr << "0";
    
    _filestr << tm->tm_sec << "] ";
    
    switch (data->module) {
      case kModAudio:   _filestr << "        AUDIO"; break;
      case kModCamera:  _filestr << "       CAMERA"; break;
      case kModConfig:  _filestr << "CONFIGURATION"; break;
      case kModControl: _filestr << "   CONTROLLER"; break;
      case kModCore:    _filestr << "         CORE"; break;
      case kModEffects: _filestr << "      EFFECTS"; break;
      case kModFont:    _filestr << "         FONT"; break;
      case kModRender:  _filestr << "       RENDER"; break;
      case kModScript:  _filestr << "       SCRIPT"; break;
      case kModState:   _filestr << "        STATE"; break;
      case kModSystem:  _filestr << "       SYSTEM"; break;
      case kModTexture: _filestr << "      TEXTURE"; break;
      case kModVideo:   _filestr << "        VIDEO"; break;
      case kModNone:
      default: {
        _filestr << "      GENERAL";
        break;
      }
    }
    
    _filestr << " -> ";
    
    switch (data->type) {
      case kLogError: {
        _filestr << "ERROR: ";
        break;
      }
      case kLogWarning: {
        _filestr << "WARNING: ";
        break;
      }
      default: {
        // We don't add any prefix
      }
    }
    
    _filestr << data->line.c_str() << std::endl;
  }
  
  if (_history.size() == kMaxLogHistory)
    _history.erase(_history.begin());
  
  _history.push_back(*data);
  
  if (config.debugMode)
    std::cout << data->line.c_str() << std::endl; // Echo to console

  SDL_UnlockMutex(_lock);
}
  
LogData Log::_makeLogData(int color, int module, int type) {
  LogData logData;
  logData.color = color;
  logData.module = module;
  logData.type = type;
  return logData;
}

}
