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

#include "DGAudioManager.h"
#include "DGConfig.h"
#include "DGLog.h"
#include "DGSystem.h"

using namespace std;

// TODO: Watch out with the context here!

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGAudioManager::DGAudioManager() {
    log = &DGLog::getInstance();
    config = &DGConfig::getInstance();

    _isInitialized = false;
	_isRunning = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGAudioManager::~DGAudioManager() {
    // FIXME: Here it's important to determine if the
    // audio was created by Lua or by another class
    
    // Each audio object should unregister itself if
    // destructed

    if (!_arrayOfAudios.empty()) {
        vector<DGAudio*>::iterator it;
        
        it = _arrayOfAudios.begin();
        
        while (it != _arrayOfAudios.end()) {
			(*it)->unload();
            it++;
        }   
    }
    
    // Now we shut down OpenAL completely
    if (_isInitialized) {
        alcMakeContextCurrent(NULL);
        alcDestroyContext(_alContext);
        alcCloseDevice(_alDevice);
    }
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGAudioManager::clear() {
    if (_isInitialized) {
        if (!_arrayOfActiveAudios.empty()) {
            vector<DGAudio*>::iterator it;
            
            it = _arrayOfActiveAudios.begin();
            
            while (it != _arrayOfActiveAudios.end()) {
                (*it)->release();
                
                it++;
            }
        }
    }
}

void DGAudioManager::flush() {
    if (_isInitialized) {
        if (!_arrayOfActiveAudios.empty()) {
            vector<DGAudio*>::iterator it;
            
            it = _arrayOfActiveAudios.begin();
            
            while (it != _arrayOfActiveAudios.end()) {
                if ((*it)->retainCount() == 0) {
                    switch ((*it)->state()) {
                        case DGAudioPlaying:
                            (*it)->fadeOut();
                            it++;
                            break;
                        case DGAudioPaused:
                        case DGAudioStopped:
                            // TODO: Automatically flush stopped and non-retained audios after
                            // n update cycles
                            DGSystem::getInstance().suspendThread(DGAudioThread);
                            (*it)->unload();
                            _arrayOfActiveAudios.erase(it);
                            DGSystem::getInstance().resumeThread(DGAudioThread);
                            break;
                        default:
                            it++;
                            break;
                    }
                }
                else it++;
            }
        }
    }
}

void DGAudioManager::init() {
    log->trace(DGModAudio, "%s", DGMsg070000);
    
    system = &DGSystem::getInstance();
    _alDevice = alcOpenDevice(NULL);
    
    if (!_alDevice) {
        log->error(DGModAudio, "%s", DGMsg270001);
        return;
    }
    
	_alContext = alcCreateContext(_alDevice, NULL);
    
    if (!_alContext) {
        log->error(DGModAudio, "%s", DGMsg270002);
        
        return;
    }
    
	alcMakeContextCurrent(_alContext);
    
    ALfloat listenerPos[] = {0.0, 0.0, 0.0};
	ALfloat listenerVel[] = {0.0, 0.0, 0.0};
	ALfloat listenerOri[] = {0.0, 0.0, -1.0, 
                             0.0, 1.0, 0.0}; // Listener facing into the screen
    
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
    
    ALint error = alGetError();
    
	if (error != AL_NO_ERROR) {
		log->error(DGModAudio, "%s: init (%d)", DGMsg270003, error);
		return;
	}
    
    _isInitialized = true;
	_isRunning = true;
}

void DGAudioManager::registerAudio(DGAudio* target) {
    _arrayOfAudios.push_back(target);
}

void DGAudioManager::requestAudio(DGAudio* target) {
    if (!target->isLoaded()) {
        target->load();
    }

    target->retain();
    
    // If the audio is not active, then it's added to
    // that vector
    
    bool isActive = false;
    std::vector<DGAudio*>::iterator it;
    it = _arrayOfActiveAudios.begin();
    
    while (it != _arrayOfActiveAudios.end()) {
        if ((*it) == target) {
            isActive = true;
            break;
        }
        
        it++;
    }
    
    DGSystem::getInstance().suspendThread(DGAudioThread);
    if (!isActive)
        _arrayOfActiveAudios.push_back(target);
    DGSystem::getInstance().resumeThread(DGAudioThread);
    
    // FIXME: Not very elegant. Must implement a state condition for
    // each audio object. Perhaps use AL_STATE even.
    if (target->state() == DGAudioPaused) {
        target->play();
    }
}

void DGAudioManager::setOrientation(float* orientation) {
    if (_isInitialized) {
        alListenerfv(AL_ORIENTATION, orientation);
    }
}

void DGAudioManager::terminate() {
	_isRunning = false;
}

// Asynchronous method
bool DGAudioManager::update() {
    if (_isRunning) {
        if (!_arrayOfActiveAudios.empty()) {
            vector<DGAudio*>::iterator it;
            
            it = _arrayOfActiveAudios.begin();
            
            while (it != _arrayOfActiveAudios.end()) {
                (*it)->update();
                it++;
            }
        }

		return true;
    }

	return false;
}
