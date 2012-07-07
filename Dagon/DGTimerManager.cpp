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
#include "DGSystem.h"
#include "DGTimerManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGTimerManager::DGTimerManager() {
    _handles = 0;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGTimerManager::~DGTimerManager() {
    // Nothing to do here
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

bool DGTimerManager::checkManual(int handle) {
    DGTimer* timer = _lookUp(handle);
    
    time_t currentTime = DGSystem::getInstance().wallTime();
    double duration = (double)(currentTime - timer->lastTime) / CLOCKS_PER_SEC;
    
    if (duration > timer->trigger) {
        timer->lastTime = currentTime;
        return true;
    }
    
    return false;
}

// TODO: Trigger should always be above 1 second
int DGTimerManager::create(double trigger, int handlerForLua) {
    DGTimer timer;
    
    timer.handle = _handles;
    timer.hasTriggered = false;
    timer.isEnabled = true;
    timer.isLoopable = true;
    timer.lastTime = DGSystem::getInstance().wallTime();
    timer.type = DGTimerNormal;    
    
    // IMPORTANT: Trigger is divided by 10 to comply with the current update scheme.
    // For precision above 1 second, control would have to be updated often.
    // Actually, it looks like the math here is 10 per pair of cores. ie: 20 for
    // quad-core. Why the hell is that?
    timer.trigger = trigger;
    timer.luaHandler = handlerForLua;
    
    _arrayOfTimers.push_back(timer);
    
    _handles++;

    return timer.handle;
}

int DGTimerManager::createInternal(double trigger, void (*callback)()) {
    DGTimer timer;
    
    timer.handle = _handles;
    timer.hasTriggered = false;
    timer.handler = callback;
    timer.isEnabled = true;
    timer.isLoopable = false;  
    timer.lastTime = DGSystem::getInstance().wallTime();
    timer.type = DGTimerInternal;
    
    timer.trigger = trigger;
    
    _arrayOfTimers.push_back(timer);
    
    _handles++;
    
    return timer.handle;
}

int DGTimerManager::createManual(double trigger) {
    DGTimer timer;
    
    timer.handle = _handles;
    timer.hasTriggered = false;
    timer.isEnabled = true;
    timer.isLoopable = false;   
    timer.lastTime = DGSystem::getInstance().wallTime();
    timer.type = DGTimerManual;
    
    timer.trigger = trigger;
    
    _arrayOfTimers.push_back(timer);
    
    _handles++;
    
    return timer.handle;
}

void DGTimerManager::destroy(int handle) {
    // Must implement
}

void DGTimerManager::disable(int handle) {
    DGTimer* timer = _lookUp(handle);
    timer->isEnabled = false;
}

void DGTimerManager::enable(int handle) {
    DGTimer* timer = _lookUp(handle);
    timer->isEnabled = true;    
}

void DGTimerManager::process() {
    std::vector<DGTimer>::iterator it;
    
    it = _arrayOfTimers.begin();
    
    while (it != _arrayOfTimers.end()) {
        if ((*it).isEnabled && ((*it).type != DGTimerManual)) {
            if ((*it).hasTriggered) {
                (*it).hasTriggered = false;
                (*it).lastTime = DGSystem::getInstance().wallTime();
                
                switch ((*it).type) {
                    case DGTimerInternal:
                        (*it).handler();
                        break;
                        
                    case DGTimerNormal:
                        DGScript::getInstance().processCallback((*it).luaHandler, 0);
                        break;
                }
                
                break; // In case the vector has changed, break the loop
            }
        }
        
        it++;
    }
}

// FIXME: This is quite sucky. Timers keep looping and being checked even if they
// were already triggered. Should have different arrays here.
void DGTimerManager::update() {
    std::vector<DGTimer>::iterator it;
    
    it = _arrayOfTimers.begin();
    
    while (it != _arrayOfTimers.end()) {
        time_t currentTime = DGSystem::getInstance().wallTime();
        double duration = (double)(currentTime - (*it).lastTime) / CLOCKS_PER_SEC;
        
        if ((*it).isEnabled && ((*it).type != DGTimerManual)) {
            if ((duration > (*it).trigger) && !(*it).hasTriggered) {
                (*it).hasTriggered = true;
            }
        }
        
        it++;
    }
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

DGTimer* DGTimerManager::_lookUp(int handle) {
    std::vector<DGTimer>::iterator it;
    
    it = _arrayOfTimers.begin();
    
    while (it != _arrayOfTimers.end()) {
        if ((*it).handle == handle)
            return &(*it);
        
        it++;
    }
    
    return NULL;
}
