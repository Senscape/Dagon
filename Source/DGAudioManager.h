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

#ifndef DG_AUDIOMANAGER_H
#define DG_AUDIOMANAGER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGAudio.h"
#include "Platform.h"

#include <AL/al.h>
#include <AL/alc.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class Config;
class Log;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGAudioManager {
    Config& config;
    Log& log;
    
    ALCdevice* _alDevice;
    ALCcontext* _alContext;
    
    std::thread _audioThread;
    
    std::mutex _mutexForArray;
    std::vector<DGAudio*> _arrayOfAudios;
    std::vector<DGAudio*> _arrayOfActiveAudios;
    
    bool _isInitialized;
	bool _isRunning;
    
    DGAudioManager();
    DGAudioManager(DGAudioManager const&);
    DGAudioManager& operator=(DGAudioManager const&);
    ~DGAudioManager();
    
public:
    static DGAudioManager& instance() {
        static DGAudioManager audioManager;
        return audioManager;
    }
    
    // These two methods have similar purposes: clear() notifies the manager
    // that the engine is about to load a new node, which prepares all
    // active audios for release. flush() effectively unloads every audio
    // that is no longer needed.
    void clear();
    void flush();
    
    void init();
    void registerAudio(DGAudio* target);
    void requestAudio(DGAudio* target);
    void setOrientation(float* orientation);
	void terminate();
    bool update();
};

#endif // DG_AUDIOMANAGER_H
