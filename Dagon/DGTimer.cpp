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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGScript.h"
#include "DGTimer.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGTimer::DGTimer() {
    _handles = 0;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGTimer::~DGTimer() {
    // Nothing to do here
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

bool DGTimer::check(int handle) {
    DGTimerData* timer = _lookUp(handle);
    
    clock_t currentTime = clock();
    double duration = (double)(currentTime - timer->lastTime) / CLOCKS_PER_SEC;
    
    if (duration > timer->trigger) {
        timer->lastTime = currentTime;
        return true;
    }
    
    return false;
}

// TODO: Trigger should always be above 1 second
int DGTimer::create(double trigger, int handlerForLua) {
    DGTimerData timer;
    
    timer.handle = _handles;
    timer.isEnabled = true;
    timer.isLoopable = true;
    timer.lastTime = clock();
    
    // IMPORTANT: Trigger is divided by 10 to comply with the current update scheme.
    // For precision above 1 second, control would have to be updated often.
    timer.trigger = trigger / 10;
    timer.luaHandler = handlerForLua;
    
    _arrayOfTimers.push_back(timer);
    
    _handles++;
    
    return timer.handle;
}


void DGTimer::destroy(int handle) {
    // Must implement
}

void DGTimer::disable(int handle) {
    DGTimerData* timer = _lookUp(handle);
    timer->isEnabled = false;
}

void DGTimer::enable(int handle) {
    DGTimerData* timer = _lookUp(handle);
    timer->isEnabled = true;    
}

void DGTimer::update() {
    std::vector<DGTimerData>::iterator it;
    
    it = _arrayOfTimers.begin();
    
    while (it != _arrayOfTimers.end()) {
        clock_t currentTime = clock();
        double duration = (double)(currentTime - (*it).lastTime) / CLOCKS_PER_SEC;
        
        if (duration > (*it).trigger && (*it).luaHandler) {
            (*it).lastTime = currentTime;
            DGScript::getInstance().callback((*it).luaHandler, 0);
        }
        
        it++;
    }
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

DGTimerData* DGTimer::_lookUp(int handle) {
    std::vector<DGTimerData>::iterator it;
    
    it = _arrayOfTimers.begin();
    
    while (it != _arrayOfTimers.end()) {
        if ((*it).handle == handle)
            return &(*it);
        
        it++;
    }
    
    return NULL;
}
