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

#ifndef DG_SYSTEM_H
#define	DG_SYSTEM_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define DGNumberOfThreads 3

enum DGThreads {
    DGAudioThread,
    DGTimerThread,
    DGVideoThread
};

class DGAudioManager;
class DGConfig;
class DGControl;
class DGLog;
class DGTimerManager;
class DGVideoManager;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGSystem {
    DGAudioManager* audioManager;
    DGControl* control;
    DGConfig* config;
    DGLog* log;
    DGTimerManager* timerManager;
    DGVideoManager* videoManager;
    
    bool _areThreadsActive;
    bool _isInitialized;
    bool _isRunning;
    
    // Private constructor/destructor
    DGSystem();
    ~DGSystem();
    // Stop the compiler generating methods of copy the object
    DGSystem(DGSystem const& copy);            // Not implemented
    DGSystem& operator=(DGSystem const& copy); // Not implemented
    
public:
    static DGSystem& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGSystem instance;
        return instance;
    }
    
    void createThreads();
    void destroyThreads();
    void findPaths(int argc, char* argv[]);
    void init();
    void resumeThread(int threadID);
    void run();
    void setTitle(const char* title);
    void suspendThread(int threadID);
    void terminate();
    void toggleFullScreen();
	void update();
    time_t wallTime();
};

#endif // DG_SYSTEM_H
