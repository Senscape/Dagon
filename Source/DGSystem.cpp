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
DGSystem::DGSystem() {  
    log = &DGLog::getInstance();
    config = &DGConfig::getInstance();

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
    log->warning(DGModSystem, "Browsing is currently disabled");
}

void DGSystem::findPaths(int argc, char* argv[]) {
    // Implement
}

void DGSystem::init() {
    if (!_isInitialized) {
        log->trace(DGModSystem, "%s", DGMsg040000);
        
        log->info(DGModControl, "%s: %s", DGMsg030000, DGVersionString);
        log->info(DGModControl, "%s: %d", DGMsg030003, DGVersionBuild);
        
        glfwSetErrorCallback(_errorCallback);
        
        if (!glfwInit())
            exit(EXIT_FAILURE);
        
        int major, minor, rev;
        glfwGetVersion(&major, &minor, &rev);
        
        log->info(DGModSystem, "%s: %d.%d.%d", DGMsg040002, major, minor, rev);
        
        // Have to make sure these are all set to 8 bits (GFW 3.0.0 defaults to 5,5,5)
        glfwWindowHint(GLFW_RED_BITS, 8);
        glfwWindowHint(GLFW_GREEN_BITS, 8);
        glfwWindowHint(GLFW_BLUE_BITS, 8);
        glfwWindowHint(GLFW_ALPHA_BITS, 8);
        
        if (config->fullScreen)
            monitor = glfwGetPrimaryMonitor();
        else
            monitor = NULL;
        
        // TODO: If width or height are zero, then attempt to get best resolution
        
        window = glfwCreateWindow(config->displayWidth, config->displayHeight, "Dagon", monitor, NULL);
        if (!window) {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        
        glfwMakeContextCurrent(window);
        
        // Set vertical sync according to our configuration
        glfwSwapInterval(config->verticalSync);
        
        // This doesn't seem to be working on windowed mode
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
       
        glfwSetCharCallback(window, _charCallback);
        glfwSetCursorPosCallback(window, _cursorPosCallback);
        glfwSetKeyCallback(window, _keyCallback);
        glfwSetMouseButtonCallback(window, _mouseButtonCallback);
        glfwSetWindowSizeCallback(window, _sizeCallback);
        
        control = &DGControl::getInstance();
        control->init();
        
        // Force a reshape to calculate the size of the cursor
        control->reshape(config->displayWidth, config->displayHeight);
        
        // Update once to show a black screen
        control->update();
        
        _isInitialized = true;
        log->trace(DGModSystem, "%s", DGMsg040001);
    }
    else log->warning(DGModSystem, "%s", DGMsg140002);
}

void DGSystem::run() {
    _isRunning = true;

    double startTime = glfwGetTime();
    double updateInterval = 1.0 / (double)config->framerate;
    
    while (!glfwWindowShouldClose(window)) {
        if (DGFramerateLimiter) {
            /* Following this game loop model:
            
            time = GetTime();
            loop
            while time < GetTime()
            UpdateGameState()
            time += update_interval
            end
             
            DrawStuff()
            SwapBuffers()
            end
            */
            while (startTime < glfwGetTime()) {
                config->setFramesPerSecond(_calculateFrames(DGFrameratePrecision));
                control->update();
            
                glfwSwapBuffers(window);
                glfwPollEvents();
            
                startTime += updateInterval;
            }
        }
        else {
            config->setFramesPerSecond(_calculateFrames(DGFrameratePrecision));
            control->update();
        
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    
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
        case 0: log->trace(DGModSystem, "%s", DGMsg040100); break;
        case 1: log->trace(DGModSystem, "%s", DGMsg040101); break;
        case 2: log->trace(DGModSystem, "%s", DGMsg040102); break;
        case 3: log->trace(DGModSystem, "%s", DGMsg040103); break;
    }
    
    glfwSetWindowShouldClose(window, GL_TRUE);
    
    _isRunning = false;
}

void DGSystem::toggleFullScreen() {
    log->warning(DGModSystem, "Toggling fullscreen currently disabled");
}

double DGSystem::wallTime() {
    return glfwGetTime();
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

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
    if (DGControl::getInstance().isConsoleActive() || character == DGKeyQuote)
        DGControl::getInstance().processKey(character, DGKeyEventDown);
}

void DGSystem::_cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    int width = DGConfig::getInstance().displayWidth;
	int height = DGConfig::getInstance().displayHeight;
    
    if (DGControl::getInstance().isDirectControlActive()) {
        if ((xpos <= 1) || ypos >= (width - 1)) {
            glfwSetCursorPos(window, width / 2, height / 2);
        }
        
        if ((xpos <= 1) || ypos >= (height - 1)) {
            glfwSetCursorPos(window, width / 2, height / 2);
        }
    }
    
    if (_isDragging)
        DGControl::getInstance().processMouse(xpos, ypos, DGMouseEventDrag);
    else
        DGControl::getInstance().processMouse(xpos, ypos, DGMouseEventMove);
}

void DGSystem::_errorCallback(int error, const char* description) {
    DGLog::getInstance().error(DGModSystem, "%s", description);
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
                DGControl::getInstance().processFunctionKey(key);
            break;
            
        // We process these keys regardless of the console state
        case GLFW_KEY_BACKSPACE:
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_TAB:
        case GLFW_KEY_ENTER:
            DGControl::getInstance().processKey(key, event);
            break;
        default:
            if (!DGControl::getInstance().isConsoleActive())
                DGControl::getInstance().processKey(key, event);
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
    DGControl::getInstance().processMouse(xpos, ypos, event);
}

void DGSystem::_sizeCallback(GLFWwindow* window, int width, int height) {
    DGControl::getInstance().reshape(width, height);
    
    // Update view for live resize
    DGControl::getInstance().update();
    glfwSwapBuffers(window);
}
