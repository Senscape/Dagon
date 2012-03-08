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

class DGAudioManager : public DGObject {
    DGConfig* config;
    DGLog* log;
    
    ALCdevice* _alDevice;
    ALCcontext* _alContext;
    std::vector<DGAudio*> _arrayOfAudios;
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
    
    void init();
    void requestNewAudio(DGAudio* target);
    void requireAudioToLoad(DGAudio* target);
    void update();
};

#endif // DG_AUDIOMANAGER_H
