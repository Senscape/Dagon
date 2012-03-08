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

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGAudioManager::DGAudioManager() {
    log = &DGLog::getInstance();
    config = &DGConfig::getInstance();
    
    _isInitialized = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGAudioManager::~DGAudioManager() {
    if (!_arrayOfAudios.empty()) {
        vector<DGAudio*>::iterator it;
        
        it = _arrayOfAudios.begin();
        
        while (it != _arrayOfAudios.end()) {
            delete *it;
            it++;
        }   
    }
    
    if (_isInitialized) {
        alcDestroyContext(_alContext);
        alcCloseDevice(_alDevice);
    }
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGAudioManager::init() {
    _alDevice = alcOpenDevice(NULL);
	_alContext = alcCreateContext(_alDevice, NULL);
	alcMakeContextCurrent(_alContext);
    
    ALint error = alGetError();
    
	if (error != AL_NO_ERROR) {
		log->error(DGModAudio, "Error while initializing OpenAL: %d", error);
        
		return;
	}
    
    ///////////////////////////////////////////
    
    DGAudio* audio;
    audio = new DGAudio;
    audio->setResource("trck_daniel_layer0.ogg");
    this->requestNewAudio(audio);
    this->requireAudioToLoad(audio);
    audio->play();
    
    ///////////////////////////////////////////
    
    _isInitialized = true;
}

void DGAudioManager::requestNewAudio(DGAudio* target) {
    _arrayOfAudios.push_back(target);
}

void DGAudioManager::requireAudioToLoad(DGAudio* target) {
    if (!target->isLoaded()) {
        target->load();
    } 
}

void DGAudioManager::update() {
    if (_isInitialized) {
        if (!_arrayOfAudios.empty()) {
            vector<DGAudio*>::iterator it;
            
            it = _arrayOfAudios.begin();
            
            while (it != _arrayOfAudios.end()) {
                (*it)->update();
                it++;
            }
        }
    }
}
