////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2012 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
//
////////////////////////////////////////////////////////////

#ifndef DG_TIMER_H
#define DG_TIMER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

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
    time_t lastTime;    
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
    std::vector<DGTimer> _arrayOfTimers;
    int _handles; // Maintains a count of handles
    int _luaObject;

	bool _isRunning;
    
    DGTimer* _lookUp(int handle);
    
    // Private constructor/destructor
    DGTimerManager();
    ~DGTimerManager();
    // Stop the compiler generating methods of copy the object
    DGTimerManager(DGTimerManager const& copy);            // Not implemented
    DGTimerManager& operator=(DGTimerManager const& copy); // Not implemented
    
public:
    static DGTimerManager& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGTimerManager instance;
        return instance;
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
	void terminate();
    bool update();
};

#endif // DG_TIMER_H
