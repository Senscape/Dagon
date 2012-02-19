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

typedef struct {
    int handle;
    bool isEnabled;
    bool isLoopable;
    clock_t lastTime;    
    int luaHandler;
    double trigger;
} DGTimerData;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGTimer {
    std::vector<DGTimerData> _arrayOfTimers;
    int _handles; // Maintains a count of handles
    
    DGTimerData* _lookUp(int handle);
    
public:
    DGTimer();
    ~DGTimer();
    
    bool check(int handle);
    int create(double trigger, int handlerForLua); // Returns a handle
    void destroy(int handle);    
    void disable(int handle);
    void enable(int handle);
    void update();
};

#endif // DG_TIMER_H
