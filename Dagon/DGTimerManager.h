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
    DGTimerNormal,
};

typedef struct {
    int handle;  
    bool isEnabled;
    bool isLoopable;
    clock_t lastTime;    
    int luaHandler;
    void (*handler)();
    double trigger;
    int type;    
} DGTimer;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGTimerManager {
    std::vector<DGTimer> _arrayOfTimers;
    int _handles; // Maintains a count of handles
    
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
    
    bool check(int handle);
    int create(double trigger, int handlerForLua); // Returns a handle
    int createInternal(double trigger, void (*callback)()); // For timers created by the engine (not Lua)
    int createManual(double trigger); // For timers that are manually checked
    void destroy(int handle);    
    void disable(int handle);
    void enable(int handle);
    void update();
};

#endif // DG_TIMER_H
