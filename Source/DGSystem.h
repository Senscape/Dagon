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

class DGConfig;
class DGLog;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGSystem {
    DGConfig& config;
    DGLog& log;

    bool _isInitialized = false;

    GLFWmonitor* monitor;
    GLFWwindow* window;
    
    double _calculateFrames(double theInterval);
    static void _charCallback(GLFWwindow* window, unsigned int character);
    static void _closeCallback(GLFWwindow* window);    
    static void _cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void _errorCallback(int error, const char* description);
    static void _keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void _mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void _sizeCallback(GLFWwindow* window, int width, int height);
    
public:
    DGSystem(DGConfig& theConfig, DGLog& theLog) :
        config(theConfig),
        log(theLog) {};
    ~DGSystem() {};
    
    void browse(const char* url);
    void findPaths();
    bool init();
    void setTitle(const char* title);
    void terminate();
    void toggleFullscreen();
    void update();
    double time();
};

#endif // DG_SYSTEM_H
