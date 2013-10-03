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

#ifndef DAGON_CONSOLE_H_
#define DAGON_CONSOLE_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Platform.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define ConsoleMargin     10
#define ConsoleRows       25
#define ConsoleSpacing    6
#define ConsoleSpeed      10
#define DGInfoMargin        10

enum ConsoleStates {
  ConsoleHidden,
  ConsoleHiding,
  ConsoleShowing,
  ConsoleVisible
};

class CameraManager;
class Config;
class CursorManager;
class Font;
class FontManager;
class Log;
class RenderManager;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class Console {
  CameraManager& cameraManager;
  Config& config;
  CursorManager& cursorManager;
  FontManager& fontManager;
  Log& log;
  RenderManager& renderManager;
  
  Font* _font;
  
  std::string _command;
  bool _isEnabled;
  bool _isInitialized;
  bool _isReadyToProcess;
  int _offset; // Used for sliding effect
  int _size;
  int _state;
  
public:
  Console();
  ~Console();
  
  void init();
  
  void disable();
  void enable();
  bool isHidden();
  bool isEnabled();
  void toggle();
  void update();
  
  void deleteChar();
  void execute();
  void getCommand(char* pointerToBuffer);
  void inputChar(char aKey);
  bool isReadyToProcess();
};
  
}

#endif // DAGON_CONSOLE_H_
