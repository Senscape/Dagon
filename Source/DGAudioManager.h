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
#include "DGPlatform.h"

#ifdef DGPlatformMac

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#else

#include <AL/al.h>
#include <AL/alc.h>

#endif

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class DGConfig;
class DGLog;
class DGSystem;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGAudioManager {
    DGConfig* config;
    DGLog* log;
    DGSystem* system;
    
    ALCdevice* _alDevice;
    ALCcontext* _alContext;
    std::vector<DGAudio*> _arrayOfAudios;
    std::vector<DGAudio*> _arrayOfActiveAudios;
    
    bool _isInitialized;
	bool _isRunning;
    
    // Private constructor/destructor
    DGAudioManager();
    ~DGAudioManager();
    // Stop the compiler generating methods of copy the object
    DGAudioManager(DGAudioManager const& copy);            // Not implemented
    DGAudioManager& operator=(DGAudioManager const& copy); // Not implemented
    
public:
    static DGAudioManager& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGAudioManager instance;
        return instance;
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
