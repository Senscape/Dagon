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

#include "DGConfig.h"
#include "DGLog.h"
#include "DGVideoManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGVideoManager::DGVideoManager() {
    log = &DGLog::getInstance();
    config = &DGConfig::getInstance();
    
    _isInitialized = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGVideoManager::~DGVideoManager() {
    // WARNING: This code assumes videos are never created
    // directly in the script
    if (!_arrayOfVideos.empty()) {
        vector<DGVideo*>::iterator it;
     
        it = _arrayOfVideos.begin();
     
        while (it != _arrayOfVideos.end()) {
            delete *it;
            it++;
        } 
     }
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGVideoManager::flush() {
    if (_isInitialized) {
        if (!_arrayOfActiveVideos.empty()) {
            vector<DGVideo*>::iterator it;
            
            it = _arrayOfActiveVideos.begin();
            
            while (it != _arrayOfActiveVideos.end()) {
                if ((*it)->retainCount() == 0) {
                    (*it)->unload();
                    _arrayOfActiveVideos.erase(it);
                }
                else it++;
            }
        }
    }
}

void DGVideoManager::init() {
    log->trace(DGModVideo, "%s", DGMsg080000);
    
    // Eventually lots of Theora initialization will be moved here
    
    _isInitialized = true;
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
    
    if (!isActive)
        _arrayOfActiveVideos.push_back(target);
}

void DGVideoManager::update() {
    if (_isInitialized) {
        if (!_arrayOfActiveVideos.empty()) {
            vector<DGVideo*>::iterator it;
            //printf("Active videos: %u\n", _arrayOfActiveVideos.size());
            it = _arrayOfActiveVideos.begin();
            
            while (it != _arrayOfActiveVideos.end()) {
                (*it)->update();
                it++;
            }
        }
    }
}
