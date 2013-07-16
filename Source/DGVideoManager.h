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
