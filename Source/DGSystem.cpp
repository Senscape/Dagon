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

#include "Config.h"
#include "DGControl.h"
#include "Log.h"
#include "DGSystem.h"

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGSystem::browse(const char* url) {
    // TODO: Re-implement
    log.warning(kModSystem, "Browsing is currently disabled");
}

void DGSystem::findPaths() {
    // TODO: Re-implement
}

bool DGSystem::init() {
    if (!_isInitialized) {
        log.trace(kModSystem, "%s", kString13001);
        
        glfwSetErrorCallback(_errorCallback);
        
        if (!glfwInit())
            return false;
        
        int major, minor, rev;
        glfwGetVersion(&major, &minor, &rev);
        
        log.info(kModSystem, "%s: %d.%d.%d", kString13003, major, minor, rev);
        
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
            
            if (config.fullscreen) {
                config.displayWidth = videoMode->width;
                config.displayHeight = videoMode->height;
            }
            else {
                // Use a third of the screen
                config.displayWidth = videoMode->width / 1.5;
                config.displayHeight = videoMode->height / 1.5;
            }
        }
        
        if (config.fullscreen)
            window = glfwCreateWindow(config.displayWidth, config.displayHeight, "Dagon", monitor, NULL);
        else
            window = glfwCreateWindow(config.displayWidth, config.displayHeight, "Dagon", NULL, NULL);
        
        if (!window) {
            glfwTerminate();
            return false;
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
        glfwSetWindowCloseCallback(window, _closeCallback);
        
        _isInitialized = true;
    }
    else log.warning(kModSystem, "%s", kString13004);
    
    return true;
}

void DGSystem::setTitle(const char* title) {
    glfwSetWindowTitle(window, title);
}

void DGSystem::update() {
    config.setFramesPerSecond(_calculateFrames( kFrameratePrecision));
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void DGSystem::terminate() {
    _isInitialized = false;
    
    glfwDestroyWindow(window);
    glfwTerminate();
}

void DGSystem::toggleFullscreen() {
    // TODO: Re-implement when GLFW 3.1 is ready
    log.warning(kModSystem, "Toggling fullscreen currently disabled");
}

double DGSystem::time() {
    return glfwGetTime();
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

// TODO: For best precision, this should be suspended when
// performing a switch (the most expensive operation)
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
    if (DGControl::instance().isConsoleActive() || character == kKeyQuote)
        DGControl::instance().processKey(character, DGEventKeyDown);
}

void DGSystem::_closeCallback(GLFWwindow* window) {
    // Simulate an escape key event
  // FIXME: This should terminate the app immediately
    DGControl::instance().processKey(kKeyEscape, DGEventKeyDown);
}

void DGSystem::_cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    int width = Config::instance().displayWidth;
	int height = Config::instance().displayHeight;
    
    if (DGControl::instance().isDirectControlActive()) {
        if ((xpos <= 1) || ypos >= (width - 1)) {
            glfwSetCursorPos(window, width / 2, height / 2);
        }
        
        if ((xpos <= 1) || ypos >= (height - 1)) {
            glfwSetCursorPos(window, width / 2, height / 2);
        }
    }
    
    DGControl::instance().processMouse(xpos, ypos, DGEventMouseMove);
}

void DGSystem::_errorCallback(int error, const char* description) {
    Log::instance().error(kModSystem, "%s", description);
}

void DGSystem::_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    int event;
    
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (mods)
            event = DGEventKeyModified;
        else
            event = DGEventKeyDown;
    }
    else if (action == GLFW_RELEASE) {
        event = DGEventKeyUp;
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
            if (event == DGEventKeyDown)
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
    int event = DGEventMouseMove;
    double xpos, ypos;
    
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            event = DGEventMouseDown;
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
            event = DGEventMouseRightDown;
    }
    else if (action == GLFW_RELEASE) {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            event = DGEventMouseUp;
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
            event = DGEventMouseRightUp;
    }
    
    glfwGetCursorPos(window, &xpos, &ypos);
    DGControl::instance().processMouse(xpos, ypos, event);
}

void DGSystem::_sizeCallback(GLFWwindow* window, int width, int height) {
    DGControl::instance().reshape(width, height);
}
