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

#include "CameraManager.h"
#include "Config.h"
#include "Console.h"
#include "CursorManager.h"
#include "Log.h"
#include "FontManager.h"
#include "RenderManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Console::Console() :
cameraManager(CameraManager::instance()),
config(Config::instance()),
cursorManager(CursorManager::instance()),
fontManager(FontManager::instance()),
log(Log::instance()),
renderManager(RenderManager::instance())
{
  _command = "";
  
  _isEnabled = false;
  _isInitialized = false;
  _isReadyToProcess = false;
  _size = ConsoleRows * (kDefFontSize + ConsoleSpacing);
  _offset = _size;
  
  _state = ConsoleHidden;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

Console::~Console() {
  // Nothing to do here
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void Console::init() {
  log.trace(kModNone, "%s", kString12003);
  _font = fontManager.loadDefault();
  _isInitialized = true;
}

void Console::deleteChar() {
  if (_command.size () > 0)
    _command.resize(_command.size () - 1);
}

void Console::disable() {
  _isEnabled = false;
}

void Console::enable() {
  if (_isInitialized)
    _isEnabled = true;
}

void Console::execute() {
  log.command(kModScript, _command.c_str());
  _isReadyToProcess = true;
}

void Console::getCommand(char* pointerToBuffer) {
  strncpy(pointerToBuffer, _command.c_str(), kMaxLogLength);
  _command.clear();
  _isReadyToProcess = false;
}

void Console::inputChar(char aKey) {
  if (_command.size() < kMaxLogLength)
    _command.insert(_command.end(), aKey);
}

bool Console::isEnabled() {
  return _isEnabled;
}

bool Console::isHidden() {
  if (_state == ConsoleHidden)
    return true;
  else
    return false;
}

bool Console::isReadyToProcess() {
  return _isReadyToProcess;
}

void Console::toggle() {
  switch (_state) {
    case ConsoleHidden:
    case ConsoleHiding:
      _state = ConsoleShowing;
      break;
    case ConsoleShowing:
    case ConsoleVisible:
      _state = ConsoleHiding;
      break;
  }
}

void Console::update() {
  if (_isEnabled) {
    Point position = cursorManager.position();
    
    switch (_state) {
      case ConsoleHidden:
        // Set the color used for information
        renderManager.setColor(kColorBrightCyan);
        _font->print(DGInfoMargin, DGInfoMargin,
                     "Viewport size: %d x %d", config.displayWidth, config.displayHeight);
        _font->print(DGInfoMargin, (DGInfoMargin * 2) + kDefFontSize,
                     "Coordinates: (%d, %d)", (int)position.x, (int)position.y);
        _font->print(DGInfoMargin, (DGInfoMargin * 3) + (kDefFontSize * 2),
                     "Viewing angle: %2.0f", cameraManager.fieldOfView());
        _font->print(DGInfoMargin, (DGInfoMargin * 4) + (kDefFontSize * 3),
                     "FPS: %2.0f", config.framesPerSecond());
        
        break;
      case ConsoleHiding:
        if (_offset < _size)
          _offset += ConsoleSpeed;
        else
          _state = ConsoleHidden;
        break;
      case ConsoleShowing:
        if (_offset > 0 && _offset > -_size)
          _offset -= ConsoleSpeed;
        else
          _state = ConsoleVisible;
        break;
    }
    
    if (_state != ConsoleHidden) {
      LogData logData;
      int row = (ConsoleRows - 2); // Extra row saved for prompt
      
      float coords[] = { 0, -_offset,
        config.displayWidth, -_offset,
        config.displayWidth, _size - _offset,
        0, _size - _offset };
      
      // Draw the slide
      renderManager.disableTextures();
      renderManager.setColor(0x25AA0000); // TODO: Add a mask color to achieve this (ie: Red + Shade)
      renderManager.drawSlide(coords);
      renderManager.enableTextures();
      
      if (log.beginIteratingHistory()) {
        do {
          log.getCurrentLine(&logData);
          
          // Draw the current line
          renderManager.setColor(logData.color);
          _font->print(ConsoleMargin, ((ConsoleSpacing + kDefFontSize) * row) - _offset, logData.line.c_str());
          
          row--;
        } while (log.iterateHistory());
      }
      
      renderManager.setColor(kColorBrightGreen);
      _font->print(ConsoleMargin, (_size - (ConsoleSpacing + kDefFontSize)) - _offset, ">%s_", _command.c_str());
    }
  }
}
