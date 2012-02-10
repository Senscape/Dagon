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

#include "DGConfig.h"
#include "DGLog.h"

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGSystem {
    DGConfig* config;
    DGLog* log;
    
    bool _isInitialized;
    bool _isRunning;
    
    void _findPaths();
    void _update();
    
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
    
    void init(int argc, const char *argv[]);
    void run();
    void setTitle(const char* title);
    void terminate();
    void toggleFullScreen();
};

#endif // DG_SYSTEM_H
