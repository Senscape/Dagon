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

#ifndef DAGON_CONTROL_H_
#define DAGON_CONTROL_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Action.h"
#include "Event.h"
#include "Platform.h"
#include "System.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define kMaxHotKeys 13

class AudioManager;
class CameraManager;
class Config;
class Console;
class CursorManager;
class FeedManager;
class FontManager;
class Interface;
class Log;
class Node;
class Overlay;
class Room;
class RenderManager;
class Scene;
class Script;
class Spot;
class State;
class TextureManager;
class TimerManager;
class VideoManager;

typedef struct {
  bool hasEnterNode;
  int enterNode;
  bool hasLeaveNode;
  int leaveNode;
  bool hasEnterRoom;
  int enterRoom;
  bool hasLeaveRoom;
  int leaveRoom;
  bool hasPreRender;
  int preRender;
  bool hasPostRender;
  int postRender;
  bool hasMouseMove;
  int mouseMove;
  bool hasMouseButton;
  int mouseButton;
  bool hasMouseRightButton;
  int mouseRightButton;
  bool hasResize;
  int resize;
} EventHandlers;

typedef struct {
  bool enabled;
  int value;
  char line[kMaxLogLength];
} DGHotkeyData;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class Control {
  AudioManager& audioManager;
  CameraManager& cameraManager;
  Config& config;
  CursorManager& cursorManager;
  FeedManager& feedManager;
  FontManager& fontManager;
  Log& log;
  RenderManager& renderManager;
  Script& script;
  System system;
  TextureManager& textureManager;
  TimerManager& timerManager;
  VideoManager& videoManager;
  
  std::vector<Room*> _arrayOfRooms;
  Room* _currentRoom;
  
  Console* _console;
  Interface* _interface;
  Scene* _scene;
  Spot* _syncedSpot;
  State* _state;
  
  EventHandlers _eventHandlers;
  DGHotkeyData _hotkeyData[kMaxHotKeys];
  
  bool _cancelSplash;
  bool _directControlActive;
  bool _isInitialized;
  bool _isRunning;
  bool _isShowingSplash; // Move this to state manager
  bool _isShuttingDown;
  int _shutdownTimer;
  int _sleepTimer;
  
  void _processAction();
  void _updateView(int state, bool inBackground);
  
  Control();
  Control(Control const&);
  Control& operator=(Control const&);
  ~Control();
  
public:
  static Control& instance() {
    static Control control;
    return control;
  }
  
  void init(int argc, char* argv[]);
  Node* currentNode();
  Room* currentRoom();
  void cutscene(const char* fileName);
  bool isConsoleActive();
  bool isDirectControlActive();
  void lookAt(float horizontal, float vertical, bool instant, bool adjustment);
  void processFunctionKey(int aKey);
  void processKey(int aKey, int eventFlags);
  void processMouse(int x, int y, int eventFlags);
  void registerGlobalHandler(int forEvent, int handlerForLua);
  void registerHotkey(int aKey, const char* luaCommandToExecute);
  void registerObject(Object* theTarget);
  void requestObject(Object* theTarget);
  void reshape(int width, int height);
  void sleep(int forSeconds);
  void syncSpot(Spot* spot);
  void switchTo(Object* theTarget);
  void walkTo(Object* theTarget);
  void run();
  void takeSnapshot();
  void terminate();
  void update();
};
  
}

#endif // DAGON_CONTROL_H_
