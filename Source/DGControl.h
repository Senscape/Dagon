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

#ifndef DG_CONTROL_H
#define DG_CONTROL_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGAction.h"
#include "DGEvent.h"
#include "DGPlatform.h"
#include "DGSystem.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define kMaxHotKeys 13

class DGAudioManager;
class DGCameraManager;
class Config;
class DGConsole;
class DGCursorManager;
class DGFont;
class DGFeedManager;
class DGFontManager;
class DGInterface;
class Log;
class DGNode;
class DGOverlay;
class DGRoom;
class DGRenderManager;
class DGScene;
class DGScript;
class DGSpot;
class DGState;
class DGTextureManager;
class DGTimerManager;
class DGVideoManager;

typedef struct {
	bool hasEnterNode;
    int	enterNode;
	bool hasLeaveNode;
	int	leaveNode;
	bool hasEnterRoom;
    int	enterRoom;
	bool hasLeaveRoom;
	int	leaveRoom;
	bool hasPreRender;
    int	preRender;
	bool hasPostRender;
	int	postRender;
	bool hasMouseMove;
    int	mouseMove;
	bool hasMouseButton;
	int	mouseButton;
	bool hasMouseRightButton;
	int	mouseRightButton;
	bool hasResize;
	int	resize;
} DGEventHandlers;

typedef struct {
	bool enabled;
	int value;
	char line[kMaxLogLength];
} DGHotkeyData;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGControl {
    DGAudioManager& audioManager;
    DGCameraManager& cameraManager;
    Config& config;
    DGCursorManager& cursorManager;
    DGFeedManager& feedManager;
    DGFontManager& fontManager;
    Log& log;
    DGRenderManager& renderManager;
    DGScript& script;
    DGTextureManager& textureManager;    
    DGTimerManager& timerManager;
    DGVideoManager& videoManager;
    
    DGSystem system;
    
    std::vector<DGRoom*> _arrayOfRooms;
    DGRoom* _currentRoom;
    
    DGConsole* _console;
    DGInterface* _interface;
    DGScene* _scene;   
    DGSpot* _syncedSpot;
    DGState* _state;
    
    DGEventHandlers _eventHandlers;
    DGHotkeyData _hotkeyData[kMaxHotKeys];
    
    bool _cancelSplash;
    bool _directControlActive;
    bool _isInitialized;
	bool _isRunning;
    bool _isShowingSplash = false; // Move this to state manager
    bool _isShuttingDown;
	int _shutdownTimer;
    int _sleepTimer;

    void _processAction();
    void _updateView(int state, bool inBackground);
    
    DGControl();
    DGControl(DGControl const&);
    DGControl& operator=(DGControl const&);
    ~DGControl();
    
public:
    static DGControl& instance() {
        static DGControl control;
        return control;
    }
    
    void init();
    DGNode* currentNode();
    DGRoom* currentRoom();
    void cutscene(const char* fileName);
	bool isConsoleActive();    
	bool isDirectControlActive();
    void lookAt(float horizontal, float vertical, bool instant);
    void processFunctionKey(int aKey);
    void processKey(int aKey, int eventFlags);
    void processMouse(int x, int y, int eventFlags);
    void registerGlobalHandler(int forEvent, int handlerForLua);
    void registerHotkey(int aKey, const char* luaCommandToExecute);
    void registerObject(DGObject* theTarget);
    void requestObject(DGObject* theTarget);
    void reshape(int width, int height);
    void sleep(int forSeconds);
    void syncSpot(DGSpot* spot);
    void switchTo(DGObject* theTarget, bool instant = false);
    void run();
    void takeSnapshot();
    void terminate();
    void update();
};

#endif // DG_CONTROL_H
