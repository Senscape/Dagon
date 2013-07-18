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

#ifndef DG_AUDIO_H
#define DG_AUDIO_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGConfig.h"
#include "DGLog.h"
#include "DGPlatform.h"

#include <AL/al.h>
#include <AL/alc.h>

#define OV_EXCLUDE_STATIC_CALLBACKS
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define DGAudioNumberOfBuffers  2

enum DGAudioFlags {
    DGAudioFadeIn,
    DGAudioFadeOut
};

enum DGAudioStates {
    DGAudioInitial,
    DGAudioPlaying,
    DGAudioPaused,
    DGAudioStopped
};

typedef struct {
    char name[DGMaxFileLength];
    char* data;
    size_t dataRead;
    size_t dataSize;  
    int index;
} DGResource;

class DGConfig;
class DGLog;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGAudio : public DGObject {
    DGConfig* config;
    DGLog* log;
    
    DGAudio* _matchedAudio;
    
    ALuint _alBuffers[DGAudioNumberOfBuffers];
	ALenum _alFormat;    
    ALuint _alSource;
    
    int _channels;
    ALsizei _rate;
    
    ov_callbacks _oggCallbacks;
    OggVorbis_File _oggStream;
    
    // Eventually all file management will be handled by a DGResourceManager class
    DGResource _resource;
    
    bool _isLoaded;
    bool _isLoopable;
    bool _isMatched;

    std::mutex _mutex;
    int _state;
    
    // Method used to fill buffers
    bool _stream(ALuint* buffer);
    
    // We use this one to periodically check for errors
    void _emptyBuffers();
    const char* _randomizeFile(const char* fileName);    
    ALboolean _verifyError(const char* operation);
    
    // Callbacks for Vorbisfile library
    static size_t _oggRead(void* ptr, size_t size, size_t nmemb, void* datasource);
    static int _oggSeek(void* datasource, ogg_int64_t offset, int whence);
    static int _oggClose(void* datasource);
    static long _oggTell(void* datasource);
    
public:
    DGAudio();
    ~DGAudio();
    
    // Checks
    
    bool isLoaded();
    bool isLoopable();
    bool isPlaying();
    
    // Gets
    
    double cursor(); // For match function
    int state();
    
    // Sets
    
    void setLoopable(bool loopable);
    void setPosition(unsigned int onFace, DGPoint origin);
    void setResource(const char* fromFileName);
    
    // State changes
    
    void load();
    void match(DGAudio* matchedAudio);
    void play();
    void pause();
    void stop();
    void unload();
    void update();
};

#endif // DG_AUDIO_H
