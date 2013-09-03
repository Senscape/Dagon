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

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <iostream>

#include "Colors.h"
#include "Config.h"
#include "Log.h"
#include "Object.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Log::Log() :
  config(Config::instance()) {
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

Log::~Log() {
  if (_filestr.is_open())
    _filestr.close();
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void Log::command(int forModule, const std::string &theString, ...) {
  char buffer[kMaxLogLength];
  va_list args;

  va_start(args, theString);
  vsnprintf(buffer, kMaxLogLength, theString.c_str(), args);
  va_end(args);
	
  LogData data;
  data.line = buffer;
	data.color = kColorBrightMagenta;
	data.module = forModule;
	data.type = kLogTrace;
	_log(&data);
}

void Log::error(int forModule, const std::string &theString, ...) {
  char buffer[kMaxLogLength];
  va_list args;
  
  va_start(args, theString);
  vsnprintf(buffer, kMaxLogLength, theString.c_str(), args);
  va_end(args);
	
  LogData data;
  data.line = buffer;
	data.color = kColorBrightRed;
	data.module = forModule;
	data.type = kLogError;
	_log(&data); 
}

void Log::info(int forModule, const std::string &theString, ...) {
  char buffer[kMaxLogLength];
  va_list args;
  
  va_start(args, theString);
  vsnprintf(buffer, kMaxLogLength, theString.c_str(), args);
  va_end(args);
	
  LogData data;
  data.line = buffer;
	data.color = kColorBrightCyan;
	data.module = forModule;
	data.type = kLogTrace;
	_log(&data);
}

void Log::trace(int forModule, const std::string &theString, ...) {
  char buffer[kMaxLogLength];
  va_list args;
  
  va_start(args, theString);
  vsnprintf(buffer, kMaxLogLength, theString.c_str(), args);
  va_end(args);
	
  LogData data;
  data.line = buffer;
	data.color = kColorWhite;
	data.module = forModule;
	data.type = kLogTrace;
	_log(&data); 
}

void Log::warning(int forModule, const std::string &theString, ...) {
  char buffer[kMaxLogLength];
  va_list args;
  
  va_start(args, theString);
  vsnprintf(buffer, kMaxLogLength, theString.c_str(), args);
  va_end(args);
	
  LogData data;
  data.line = buffer;
	data.color = kColorYellow;
	data.module = forModule;
	data.type = kLogWarning;
	_log(&data);
}

////////////////////////////////////////////////////////////
// Implementation - Console helpers
////////////////////////////////////////////////////////////

bool Log::beginIteratingHistory() {
  if (!_history.empty()) {
    _it = _history.rbegin();
    return true;
  }
  return false;
}

bool Log::iterateHistory() {
  ++_it;
  if (_it == (_history.rend() - 1) || _history.empty()) {
    return false; // Bypass the first line
  } else {
    return true;
  }
}

void Log::getCurrentLine(LogData* pointerToLogData) {
  assert(_it != _history.rend());
  pointerToLogData->line = _it->line;
  pointerToLogData->color = _it->color;
  pointerToLogData->module = _it->module;
  pointerToLogData->type = _it->type;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void Log::_log(LogData* data) {
  if (config.log) {
    if (!_filestr.is_open())
      _filestr.open(config.path(kPathUserData, kDefLogFile,
                                kObjectGeneric), std::ofstream::app);
      
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
    
    _filestr << data->line << std::endl;
  }
  
  if (_history.size() == kMaxLogHistory)
    _history.erase(_history.begin());
  
  _history.push_back(*data);
  
  if (config.debugMode)
    std::cout << data->line << std::endl; // Echo to console
}
