////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
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

#define DGMaxHotKeys 12

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

enum DGEvents {
	DGEventEnterNode = 1, // Must implement
	DGEventLeaveNode, // Must implement
	DGEventEnterRoom, // Must implement
	DGEventLeaveRoom, // Must implement
	DGEventPreRender, // Must implement
	DGEventPostRender,
	DGEventMouseButton,	
	DGEventMouseMove,
	DGEventResize
};

enum DGMouseEvents {
    DGMouseEventDown = 0x1,    
    DGMouseEventDrag = 0x2,
    DGMouseEventMove = 0x4,
    DGMouseEventUp = 0x8,
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
	bool hasResize;
	int	resize;	
} DGEventHandlers;

typedef struct {
	bool enabled;
	int value;
	char line[DGMaxLogLength];
} DGHotKeyData;

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
    DGRenderManager* render;
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
    DGHotKeyData _hotKeyData[DGMaxHotKeys];
    
    int _fpsCount;
    bool _isInitialized;
    bool _isShuttingDown;
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
    void lookAt(float horizontal, float vertical, bool instant);
    void processFunctionKey(int aKey);
    void processKey(int aKey, bool isModified);
    void processMouse(int x, int y, int eventFlags);
    void registerGlobalHandler(int forEvent, int handlerForLua);
    void registerHotKey(int aKey, const char* luaCommandToExecute);
    void registerObject(DGObject* theTarget);    
    void reshape(int width, int height);
    void sleep(int forSeconds);
    // TODO: Add an explicit switchToNode() method
    void syncSpot(DGSpot* spot);
    void switchTo(DGObject* theTarget); 
    void takeSnapshot();
    static void terminate();
    
    // These methods are called asynchronously
    void profiler();
    void update();
};

#endif // DG_CONTROL_H
