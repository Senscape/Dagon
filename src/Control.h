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

#include <unordered_map>
#include <forward_list>

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define kMaxHotKeys 20

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

struct GraphNode {
  Node* node;
  std::forward_list<GraphNode*> adj;

  GraphNode() {
    node = NULL;
  }

  void addEdgeTo(const GraphNode& to) {
    GraphNode* needle = (GraphNode*)&to;
    for (auto it = adj.begin(); it != adj.end(); ++it) {
      if (*it == needle)
	return;
    }

    adj.push_front(needle);
  }
};

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
  int _prevControlMode;
  
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

  std::unordered_map<Object*, uint64_t> objMap;
  std::unordered_map<uint64_t, Object*> invObjMap;
  std::forward_list<GraphNode> controlGraph;
  
  void init(int argc, char* argv[]);
  Node* currentNode();
  Room* currentRoom();
  Room* assetRoom() const;
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
  void reshape(int width, int height);
  void sleep(int forSeconds);
  void syncSpot(Spot* spot);
  void switchTo(Object* theTarget);
  void walkTo(Object* theTarget);
  void run();
  void takeSnapshot();
  void initiateTerminate();
  void terminate();
  void update();
  std::vector<Room*> rooms();
  void addLinkFromTo(Node* from, Node* to);
};
  
}

#endif // DAGON_CONTROL_H_
