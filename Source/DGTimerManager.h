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

#ifndef DG_TIMERMANAGER_H
#define DG_TIMERMANAGER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Platform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class DGSystem;

enum DGTimerTypes {
    DGTimerInternal,
    DGTimerManual,
    DGTimerNormal
};

typedef struct {
    int handle;  
    bool isEnabled;
    bool isLoopable;
    bool hasTriggered;
    double lastTime;
    int luaHandler;
    void (*handler)();
    int luaObject;
    double trigger;
    int type;    
} DGTimer;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGTimerManager {
    DGSystem* system;
  
    std::vector<DGTimer> _arrayOfTimers;
    int _handles; // Maintains a count of handles
    int _luaObject;

	bool _isRunning;
    
    std::mutex _mutexForArray;
    std::thread _timerThread;
    DGTimer* _lookUp(int handle);
    
    DGTimerManager();
    DGTimerManager(DGTimerManager const&);
    DGTimerManager& operator=(DGTimerManager const&);
    ~DGTimerManager();
    
public:
    static DGTimerManager& instance() {
        static DGTimerManager timerManager;
        return timerManager;
    }
    
    bool checkManual(int handle);
    int create(double trigger, bool shouldLoop, int handlerForLua, int luaObject = 0); // Returns a handle
    int createInternal(double trigger, void (*callback)()); // For timers created by the engine (not Lua)
    int createManual(double trigger); // For timers that are manually checked by the user
    void destroy(int handle);    
    void disable(int handle);
    void enable(int handle);
    void process();
    void setLuaObject(int luaObject);
    void setSystem(DGSystem* theSystem);
	void terminate();
    bool update();
};

#endif // DG_TIMERMANAGER_H
