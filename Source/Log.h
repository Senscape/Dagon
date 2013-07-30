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

#ifndef DAGON_LOG_H_
#define DAGON_LOG_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <fstream>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////

class Config;

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum LogModules {
  kModAudio,
  kModCamera,
  kModConfig,
  kModControl,
  kModCore,
  kModEffects,
  kModFont,
  kModRender,
  kModScript,
  kModState,
  kModSystem,
  kModTexture,
  kModVideo,
  kModNone
};

enum LogTypes {
	kLogTrace,
	kLogError,
	kLogWarning
};

typedef struct {
  std::string line;
  int color;
  int module;
  int type;
} LogData;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class Log {
 public:
  static Log& instance() {
    static Log log;
    return log;
  }
    
  void command(int forModule, const char* theString, ...);
  void error(int forModule, const char* theString, ...);
  void info(int forModule, const char* theString, ...);
  void trace(int forModule, const char* theString, ...);
  void warning(int forModule, const char* theString, ...);
    
  // Helpers for the console feature
    
  bool beginIteratingHistory();
  bool iterateHistory();
  void getCurrentLine(LogData* pointerToLogData);
  
 private:
  Config& config;
  
  std::ofstream _filestr;
  std::vector<LogData> _history;
  std::vector<LogData>::reverse_iterator _it;
  void _log(LogData* data);
  
  Log();
  ~Log();
  Log(const Log&);
  void operator=(const Log&);
};

#endif // DAGON_LOG_H_
