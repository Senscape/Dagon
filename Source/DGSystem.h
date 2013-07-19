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

#ifndef DG_SYSTEM_H
#define	DG_SYSTEM_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define DGFrameratePrecision 0.5

class DGConfig;
class DGControl;
class DGLog;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGSystem {
    DGControl* control;
    DGConfig* config;
    DGLog* log;

    bool _isInitialized;
    bool _isRunning;
    
    GLFWmonitor* monitor;
    GLFWwindow* window;
    
    double _calculateFrames(double theInterval);
    static void _charCallback(GLFWwindow* window, unsigned int character);
    static void _cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void _errorCallback(int error, const char* description);
    static void _keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void _mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void _sizeCallback(GLFWwindow* window, int width, int height);
    
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
    
    void browse(const char* url);
    void findPaths(int argc, char* argv[]);
    void init();
    void run();
    void setTitle(const char* title);
    void terminate();
    void toggleFullScreen();
    double wallTime();
};

#endif // DG_SYSTEM_H
