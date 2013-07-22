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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGConfig.h"
#include "DGControl.h"
#include "DGLog.h"
#include "DGSystem.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// Control should decide this
int _isDragging = false;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

// TODO: At this point the system module should copy the config file
// into the user folder
DGSystem::DGSystem() :
    config(DGConfig::instance()),
    log(DGLog::instance())
{
    _isInitialized = false;
    _isRunning = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGSystem::~DGSystem() {
    // The shutdown sequence is performed in the terminate() method
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGSystem::browse(const char* url) {
    log.warning(DGModSystem, "Browsing is currently disabled");
}

void DGSystem::findPaths(int argc, char* argv[]) {
    // Implement
}

void DGSystem::init() {
    if (!_isInitialized) {
        log.trace(DGModSystem, "%s", DGMsg040000);
        
        log.info(DGModControl, "%s: %s", DGMsg030000, DGVersionString);
        log.info(DGModControl, "%s: %d", DGMsg030003, DGVersionBuild);
        
        glfwSetErrorCallback(_errorCallback);
        
        if (!glfwInit())
            exit(EXIT_FAILURE);
        
        int major, minor, rev;
        glfwGetVersion(&major, &minor, &rev);
        
        log.info(DGModSystem, "%s: %d.%d.%d", DGMsg040002, major, minor, rev);
        
        // Have to make sure these are all set to 8 bits (GFW 3.0.0 defaults to 5,5,5)
        glfwWindowHint(GLFW_RED_BITS, 8);
        glfwWindowHint(GLFW_GREEN_BITS, 8);
        glfwWindowHint(GLFW_BLUE_BITS, 8);
        glfwWindowHint(GLFW_ALPHA_BITS, 8);
        
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
        
        if (!config.displayWidth || !config.displayHeight) {
            config.displayWidth = videoMode->width;
            config.displayHeight = videoMode->height;
            
            if (config.fullScreen) {
                config.displayWidth = videoMode->width;
                config.displayHeight = videoMode->height;
            }
            else {
                // Use a third the screen
                config.displayWidth = videoMode->width / 1.5;
                config.displayHeight = videoMode->height / 1.5;
            }
        }
        
        if (config.fullScreen)
            window = glfwCreateWindow(config.displayWidth, config.displayHeight, "Dagon", monitor, NULL);
        else
            window = glfwCreateWindow(config.displayWidth, config.displayHeight, "Dagon", NULL, NULL);
        
        if (!window) {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        
        glfwMakeContextCurrent(window);
        
        // Set vertical sync according to our configuration
        if (config.verticalSync) {
            glfwSwapInterval(1);
        }
        else {
            // Force our own frame limiter on
            config.frameLimiter = true;
            glfwSwapInterval(0);
        }
        
        // This doesn't seem to be working on windowed mode
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
       
        glfwSetCharCallback(window, _charCallback);
        glfwSetCursorPosCallback(window, _cursorPosCallback);
        glfwSetKeyCallback(window, _keyCallback);
        glfwSetMouseButtonCallback(window, _mouseButtonCallback);
        glfwSetWindowSizeCallback(window, _sizeCallback);
        
        control = &DGControl::instance();
        control->init();
        
        // Force a reshape to calculate the size of the cursor
        control->reshape(config.displayWidth, config.displayHeight);
        
        // Update once to show a black screen
        control->update();
        
        _isInitialized = true;
        log.trace(DGModSystem, "%s", DGMsg040001);
    }
    else log.warning(DGModSystem, "%s", DGMsg140002);
}

void DGSystem::run() {
    _isRunning = true;

    double startTime = glfwGetTime();
    double updateInterval = 1.0 / (double)config.framerate;
    
    while (_isRunning && !glfwWindowShouldClose(window)) {
        if (config.frameLimiter) {
            while (startTime < glfwGetTime()) {
                config.setFramesPerSecond(_calculateFrames(DGFrameratePrecision));
                control->update();
            
                glfwSwapBuffers(window);
                glfwPollEvents();
            
                startTime += updateInterval;
            }
        }
        else {
            config.setFramesPerSecond(_calculateFrames(DGFrameratePrecision));
            control->update();
        
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    
    _isRunning = false;
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    exit(EXIT_SUCCESS);
}

void DGSystem::setTitle(const char* title) {
    glfwSetWindowTitle(window, title);
}

void DGSystem::terminate() {
    int r = arc4random() % 8; // Double the replies, so that the default one appears often
    
    switch (r) {
        default:
        case 0: log.trace(DGModSystem, "%s", DGMsg040100); break;
        case 1: log.trace(DGModSystem, "%s", DGMsg040101); break;
        case 2: log.trace(DGModSystem, "%s", DGMsg040102); break;
        case 3: log.trace(DGModSystem, "%s", DGMsg040103); break;
    }
    
    glfwSetWindowShouldClose(window, GL_TRUE);
}

void DGSystem::toggleFullScreen() {
    log.warning(DGModSystem, "Toggling fullscreen currently disabled");
}

double DGSystem::wallTime() {
    // FIXME: This is a shoddy way to confirm if the GLFW library is initialized
    if (_isRunning)
        return glfwGetTime();
    else
        return 0;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

// NOTE: For best precision, this should be suspended when
// performing a switch (the most expensive operation).
double DGSystem::_calculateFrames(double theInterval = 1.0) {
	static double lastTime = glfwGetTime();
	static int fpsFrameCount = 0;
	static double fps = 0.0;
    
	double currentTime = glfwGetTime();

	if (theInterval < 0.1)
		theInterval = 0.1;
    
	if (theInterval > 10.0)
		theInterval = 10.0;
    
	if ((currentTime - lastTime) > theInterval) {
		fps = (double)fpsFrameCount / (currentTime - lastTime);

		fpsFrameCount = 0;
		lastTime = glfwGetTime();
	}
	else fpsFrameCount++;

	return fps;
}

void DGSystem::_charCallback(GLFWwindow* window, unsigned int character) {
    if (DGControl::instance().isConsoleActive() || character == DGKeyQuote)
        DGControl::instance().processKey(character, DGKeyEventDown);
}

void DGSystem::_cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    int width = DGConfig::instance().displayWidth;
	int height = DGConfig::instance().displayHeight;
    
    if (DGControl::instance().isDirectControlActive()) {
        if ((xpos <= 1) || ypos >= (width - 1)) {
            glfwSetCursorPos(window, width / 2, height / 2);
        }
        
        if ((xpos <= 1) || ypos >= (height - 1)) {
            glfwSetCursorPos(window, width / 2, height / 2);
        }
    }
    
    if (_isDragging)
        DGControl::instance().processMouse(xpos, ypos, DGMouseEventDrag);
    else
        DGControl::instance().processMouse(xpos, ypos, DGMouseEventMove);
}

void DGSystem::_errorCallback(int error, const char* description) {
    DGLog::instance().error(DGModSystem, "%s", description);
}

void DGSystem::_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    int event;
    
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (mods)
            event = DGKeyEventModified;
        else
            event = DGKeyEventDown;
    }
    else if (action == GLFW_RELEASE) {
        event = DGKeyEventUp;
    }
    
    switch (key) {
        // Function keys are processed by a different method
        case GLFW_KEY_F1:
        case GLFW_KEY_F2:
        case GLFW_KEY_F3:
        case GLFW_KEY_F4:
        case GLFW_KEY_F5:
        case GLFW_KEY_F6:
        case GLFW_KEY_F7:
        case GLFW_KEY_F8:
        case GLFW_KEY_F9:
        case GLFW_KEY_F10:
        case GLFW_KEY_F11:
        case GLFW_KEY_F12:
            if (event == DGKeyEventDown)
                DGControl::instance().processFunctionKey(key);
            break;
            
        // We process these keys regardless of the console state
        case GLFW_KEY_BACKSPACE:
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_TAB:
        case GLFW_KEY_ENTER:
            DGControl::instance().processKey(key, event);
            break;
        default:
            if (!DGControl::instance().isConsoleActive())
                DGControl::instance().processKey(key, event);
            break;
    }
}

void DGSystem::_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    int event = DGMouseEventMove;
    double xpos, ypos;
    
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            event = DGMouseEventDown;
            _isDragging = true;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
            event = DGMouseEventRightDown;
    }
    else if (action == GLFW_RELEASE) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            event = DGMouseEventUp;
            _isDragging = false;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
            event = DGMouseEventRightUp;
    }
    
    glfwGetCursorPos(window, &xpos, &ypos);
    DGControl::instance().processMouse(xpos, ypos, event);
}

void DGSystem::_sizeCallback(GLFWwindow* window, int width, int height) {
    DGControl::instance().reshape(width, height);
    
    // Update view for live resize
    DGControl::instance().update();
    glfwSwapBuffers(window);
}
