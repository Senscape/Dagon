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

#ifndef DG_SYSTEM_H
#define DG_SYSTEM_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <SDL2/SDL.h>

#include "Platform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class Config;
class Log;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGSystem {
  Config& config;
  Log& log;
  
  SDL_GLContext _context;
  SDL_Window *_window;
  
  double _calculateFrames(double theInterval);
  
public:
  DGSystem(Config& theConfig, Log& theLog) :
  config(theConfig),
  log(theLog) {};
  ~DGSystem() {};
  
  void browse(const char* url);
  void findPaths();
  bool init();
  void setTitle(const char* title);
  void terminate();
  void toggleFullscreen();
  void update();
};

#endif // DG_SYSTEM_H
