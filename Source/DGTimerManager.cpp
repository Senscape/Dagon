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
    _luaObject = 0;

	_isRunning = true;
    
    // TODO: Move this to init()
    _timerThread = thread([&](){
        chrono::milliseconds dura(1);
        while (DGTimerManager::instance().update()) {
            this_thread::sleep_for(dura);
        }
    });
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGTimerManager::~DGTimerManager() {
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

bool DGTimerManager::checkManual(int handle) {
    DGTimer* timer = _lookUp(handle);
    
    if (timer->isEnabled) {
      sf::Time time = _clock.getElapsedTime();
      double currentTime = time.asSeconds();
        double duration = currentTime - timer->lastTime;
        
        if (duration > timer->trigger) {
            timer->lastTime = currentTime;
            return true;
        }
    }
    
    return false;
}

int DGTimerManager::create(double trigger, bool shouldLoop, int handlerForLua, int luaObject) {
    DGTimer timer;
    
    timer.handle = _handles;
    timer.hasTriggered = false;
    timer.isEnabled = true;
    timer.isLoopable = shouldLoop;
  sf::Time time = _clock.getElapsedTime();
    timer.lastTime = time.asSeconds();
    timer.type = DGTimerNormal;    
    
    timer.trigger = trigger;
    timer.luaHandler = handlerForLua;
    timer.luaObject = luaObject;

    _mutexForArray.lock();
    _arrayOfTimers.push_back(timer);
    _mutexForArray.unlock();
    
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
  sf::Time time = _clock.getElapsedTime();
    timer.lastTime = time.asSeconds();
    timer.type = DGTimerInternal;
    
    timer.trigger = trigger;
    
    _mutexForArray.lock();
    _arrayOfTimers.push_back(timer);
    _mutexForArray.unlock();
    
    _handles++;
    
    return timer.handle;
}

int DGTimerManager::createManual(double trigger) {
    DGTimer timer;
    
    timer.handle = _handles;
    timer.hasTriggered = false;
    timer.isEnabled = true;
    timer.isLoopable = false;
  sf::Time time = _clock.getElapsedTime();
    timer.lastTime = time.asSeconds();
    timer.type = DGTimerManual;
    
    timer.trigger = trigger;

    _mutexForArray.lock();
    _arrayOfTimers.push_back(timer);
    _mutexForArray.unlock();
    
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
  sf::Time time = _clock.getElapsedTime();
    timer->lastTime = time.asSeconds();
}

void DGTimerManager::process() {
    bool keepProcessing = true;
    
    std::vector<DGTimer>::iterator it;
    
    _mutexForArray.lock();    
    it = _arrayOfTimers.begin();
    
    while (it != _arrayOfTimers.end()) {
        if ((*it).isEnabled && ((*it).type != DGTimerManual)) {
            if ((*it).hasTriggered) {                
                switch ((*it).type) {
                    case DGTimerInternal:
                        (*it).handler();
                        keepProcessing = false;
                        break;
                        
                    case DGTimerNormal:
                        if ((*it).luaObject) { // Belongs to a Lua object?
                            if ((*it).luaObject != _luaObject) {
                                (*it).hasTriggered = false;
                              sf::Time time = _clock.getElapsedTime();
                                (*it).lastTime = time.asSeconds(); // Reset timer
                                break; // Do not invoke the handler
                            }
                        }
                        
                        if ((*it).isLoopable) {
                            (*it).hasTriggered = false;
                          sf::Time time = _clock.getElapsedTime();
                            (*it).lastTime = time.asSeconds();
                        }
                        else {
                            // Should destroy in reality
                            (*it).isEnabled = false;
                        }
                        
                        // Must unlock here for precaution
                        _mutexForArray.unlock();
                        DGScript::instance().processCallback((*it).luaHandler, 0);
                        keepProcessing = false;
                        break;
                }
                
                if (!keepProcessing)
                    break; // In case the vector has changed, break the loop
            }
        }
        
        it++;
    }
    
    _mutexForArray.unlock();
}

void DGTimerManager::setLuaObject(int luaObject) {
    _luaObject = luaObject;
}

void DGTimerManager::setSystem(DGSystem* theSystem) {
    system = theSystem;
}

void DGTimerManager::terminate() {
    _isRunning = false;
    
    _timerThread.join();
}

// FIXME: This is quite sucky. Timers keep looping and being checked even if they
// were already triggered. Should have different arrays here per each kind of timer.
bool DGTimerManager::update() {
	if (_isRunning) {
		std::vector<DGTimer>::iterator it;
    
        _mutexForArray.lock();
		it = _arrayOfTimers.begin();
    
		while (it != _arrayOfTimers.end()) {
      sf::Time time = _clock.getElapsedTime();
			double currentTime = time.asSeconds();
			double duration = currentTime - (*it).lastTime;
        
			DGTimer* timer = &(*it);
        
			if (timer->isEnabled && (timer->type != DGTimerManual)) {
				if ((duration > timer->trigger) && !timer->hasTriggered) {
					timer->hasTriggered = true;
				}
			}
        
			it++;
		}
        _mutexForArray.unlock();
        
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

DGTimer* DGTimerManager::_lookUp(int handle) {
    std::vector<DGTimer>::iterator it;
    
    _mutexForArray.lock();
    it = _arrayOfTimers.begin();
    
    while (it != _arrayOfTimers.end()) {
        if ((*it).handle == handle) {            
            _mutexForArray.unlock();
            return &(*it);
        }
        
        it++;
    }
    
    _mutexForArray.unlock();
    return NULL;
}
