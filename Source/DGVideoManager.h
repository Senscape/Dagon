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

#ifndef DG_VIDEOMANAGER_H
#define DG_VIDEOMANAGER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_thread.h>

#include "Platform.h"
#include "DGVideo.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class Config;
class Log;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGVideoManager {
    Config& config;
    Log& log;
    
    SDL_mutex* _mutex;
    SDL_Thread* _thread;
    std::vector<DGVideo*> _arrayOfVideos;
    std::vector<DGVideo*> _arrayOfActiveVideos;
    
    bool _isInitialized;
	bool _isRunning;
  
    static int _runThread(void *ptr);
    
    DGVideoManager();
    DGVideoManager(DGVideoManager const&);
    DGVideoManager& operator=(DGVideoManager const&);
    ~DGVideoManager();
    
public:
    static DGVideoManager& instance() {
        static DGVideoManager videoManager;
        return videoManager;
    }
    
    void init();
    void flush();    
    void registerVideo(DGVideo* target);
    void requestVideo(DGVideo* target);
	void terminate();
    bool update();
};

#endif // DG_VIDEOMANAGER_H
