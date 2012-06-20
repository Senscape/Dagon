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

#ifndef DG_AUDIOMANAGER_H
#define DG_AUDIOMANAGER_H

// TODO: Obviously, it makes sense to have all these managers
// inherit from a base, singleton manager

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

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGAudioManager {
    DGConfig* config;
    DGLog* log;
    
    ALCdevice* _alDevice;
    ALCcontext* _alContext;
    std::vector<DGAudio*> _arrayOfAudios;
    std::vector<DGAudio*> _arrayOfActiveAudios;
    
    bool _isInitialized;
    
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
    void update();
};

#endif // DG_AUDIOMANAGER_H
