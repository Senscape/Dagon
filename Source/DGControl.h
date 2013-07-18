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
#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define DGTimeToStartDragging 0.25f
#define DGMaxHotKeys 13

class DGAudioManager;
class DGCameraManager;
class DGConfig;
class DGConsole;
class DGCursorManager;
class DGFont;
class DGFeedManager;
class DGFontManager;
class DGInterface;
class DGLog;
class DGNode;
class DGOverlay;
class DGRoom;
class DGRenderManager;
class DGScene;
class DGScript;
class DGSpot;
class DGState;
class DGSystem;
class DGTextureManager;
class DGTimerManager;
class DGVideoManager;

enum DGEvents { // For Lua
	DGEventEnterNode = 1, // Must implement
	DGEventLeaveNode, // Must implement
	DGEventEnterRoom, // Must implement
	DGEventLeaveRoom, // Must implement
	DGEventPreRender, // Must implement
	DGEventPostRender,
	DGEventMouseButton,
	DGEventMouseRightButton,
	DGEventMouseMove,
	DGEventResize
};

enum DGKeyEvents {
    DGKeyEventDown = 0x2,
    DGKeyEventModified = 0x4,
    DGKeyEventUp = 0x6
};

enum DGMouseEvents {
    DGMouseEventDown = 0x1,    
    DGMouseEventDrag = 0x2,
    DGMouseEventMove = 0x4,
    DGMouseEventUp = 0x8,
    DGMouseEventRightDown = 0x10,
    DGMouseEventRightUp = 0x12
};

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
	char line[DGMaxLogLength];
} DGHotkeyData;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGControl {
    DGAudioManager* audioManager;
    DGCameraManager* cameraManager;    
    DGConfig* config;
    DGCursorManager* cursorManager;
    DGFeedManager* feedManager;
    DGFontManager* fontManager;
    DGLog* log;
    DGRenderManager* renderManager;
    DGScript* script;
    DGSystem* system;
    DGTimerManager* timerManager;
    DGVideoManager* videoManager;    
    
    std::vector<DGRoom*> _arrayOfRooms;
    DGRoom* _currentRoom;
    
    DGConsole* _console;
    DGInterface* _interface;
    DGScene* _scene;   
    DGSpot* _syncedSpot;
    DGState* _state;
    DGTextureManager* _textureManager;
    
    DGEventHandlers _eventHandlers;
    DGHotkeyData _hotkeyData[DGMaxHotKeys];
    
    bool _cancelSplash;
    bool _directControlActive;
    int _dragTimer;
    bool _isInitialized;
	bool _isRunning;
    bool _isShuttingDown;
	int _shutdownTimer;
    int _sleepTimer;

    void _processAction();
    void _updateView(int state, bool inBackground);
    
    // Private constructor/destructor
    DGControl();
    ~DGControl();
    // Stop the compiler generating methods of copy the object
    DGControl(DGControl const& copy);            // Not implemented
    DGControl& operator=(DGControl const& copy); // Not implemented
    
public:
    static DGControl& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGControl instance;
        return instance;
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
    void reshape(int width, int height);
    void sleep(int forSeconds);
    void syncSpot(DGSpot* spot);
    void switchTo(DGObject* theTarget, bool instant = false);
    void takeSnapshot();
    void terminate();
    bool update();
};

#endif // DG_CONTROL_H
