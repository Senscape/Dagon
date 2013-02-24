////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2012 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
//
////////////////////////////////////////////////////////////

#ifndef DG_VIDEOMANAGER_H
#define DG_VIDEOMANAGER_H

// TODO: Obviously, it makes sense to have all these managers
// inherit from a base, singleton manager

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"
#include "DGVideo.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class DGConfig;
class DGLog;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGVideoManager {
    DGConfig* config;
    DGLog* log;
    
    std::vector<DGVideo*> _arrayOfVideos;
    std::vector<DGVideo*> _arrayOfActiveVideos;
    
    bool _isInitialized;
	bool _isRunning;
    
    // Private constructor/destructor
    DGVideoManager();
    ~DGVideoManager();
    // Stop the compiler generating methods of copy the object
    DGVideoManager(DGVideoManager const& copy);            // Not implemented
    DGVideoManager& operator=(DGVideoManager const& copy); // Not implemented
    
public:
    static DGVideoManager& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGVideoManager instance;
        return instance;
    }
    
    void flush();
    
    void init();
    void registerVideo(DGVideo* target);
    void requestVideo(DGVideo* target);
	void terminate();
    bool update();
};

#endif // DG_VIDEOMANAGER_H
