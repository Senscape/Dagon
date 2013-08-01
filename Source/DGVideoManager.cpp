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

#include "Config.h"
#include "Log.h"
#include "DGVideoManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGVideoManager::DGVideoManager() :
    config(Config::instance()),
    log(Log::instance())
{
    _isInitialized = false;
	_isRunning = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGVideoManager::~DGVideoManager() {
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGVideoManager::flush() {
	bool done = false;

    if (_isInitialized) {
        if (!_arrayOfActiveVideos.empty()) {
            vector<DGVideo*>::iterator it;
            
			while (!done) {
                _mutexForArray.lock();
				it = _arrayOfActiveVideos.begin();
				done = true;
				while (it != _arrayOfActiveVideos.end()) {
					if ((*it)->retainCount() == 0) {
						(*it)->unload();
						_arrayOfActiveVideos.erase(it);
						done = false;
						break;
					}
					else it++;
				}
                _mutexForArray.unlock();
			}
        }
    }
}

void DGVideoManager::init() {
    log.trace(kModVideo, "%s", kString17001);
    log.info(kModVideo, "%s: %s", kString17006, theora_version_string());
    
    // Eventually lots of Theora initialization process will be moved here
    
    _isInitialized = true;
	_isRunning = true;
    
    _videoThread = thread([](){
        chrono::milliseconds dura(1);
        while (DGVideoManager::instance().update()) {
            this_thread::sleep_for(dura);
        }
    });
}

void DGVideoManager::registerVideo(DGVideo* target) {
    _arrayOfVideos.push_back(target);
}

void DGVideoManager::requestVideo(DGVideo* target) {
    if (!target->isLoaded()) {
        target->load();
    }
    
    // If the audio is not active, then it's added to
    // that vector
    
    bool isActive = false;
    std::vector<DGVideo*>::iterator it;
    it = _arrayOfActiveVideos.begin();
    
    while (it != _arrayOfActiveVideos.end()) {
        if ((*it) == target) {
            isActive = true;
            break;
        }
        
        it++;
    }
    
    if (!isActive) {
        _mutexForArray.lock();
        _arrayOfActiveVideos.push_back(target);
        _mutexForArray.unlock();
    }
}

void DGVideoManager::terminate() {
    _isRunning = false;
    
    _videoThread.join();
    
    // WARNING: This code assumes videos are never created
    // directly in the script
    if (!_arrayOfVideos.empty()) {
        vector<DGVideo*>::iterator it;
        
        _mutexForArray.lock();
        it = _arrayOfVideos.begin();
        
        while (it != _arrayOfVideos.end()) {
            delete *it;
            it++;
        }
        _mutexForArray.unlock();
    }
}

bool DGVideoManager::update() {
    if (_isRunning) {
        if (!_arrayOfActiveVideos.empty()) {
            vector<DGVideo*>::iterator it;
            
            _mutexForArray.lock();
            it = _arrayOfActiveVideos.begin();
            
            while (it != _arrayOfActiveVideos.end()) {
                (*it)->update();
                it++;
            }
            
            _mutexForArray.unlock();
        }

		return true;
    }

	return false;
}
