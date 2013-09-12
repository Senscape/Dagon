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
  log.trace(kModSystem, "%s", kString13001);
  SDL_version version;
  SDL_GetVersion(&version);
  log.info(kModSystem, "%s: %d.%d", kString13003,
            version.major, version.minor);
  
  SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER);
  Uint32 videoFlags = SDL_WINDOW_OPENGL;

  if (!config.displayWidth || !config.displayHeight) {
    if (config.fullscreen) {
      SDL_DisplayMode displayMode;
      SDL_GetCurrentDisplayMode(0, &displayMode);
      config.displayWidth = displayMode.w;
      config.displayHeight = displayMode.h;
      videoFlags = videoFlags | SDL_WINDOW_FULLSCREEN;
    } else {
      // Use a third of the screen
      SDL_DisplayMode desktopMode;
      SDL_GetDesktopDisplayMode(0, &desktopMode);
      config.displayWidth = static_cast<int>(desktopMode.w / 1.5f);
      config.displayHeight = static_cast<int>(desktopMode.h / 1.5f);
      videoFlags = videoFlags | SDL_WINDOW_RESIZABLE;
    }
  } else {
    if (config.fullscreen) {
      videoFlags = videoFlags | SDL_WINDOW_FULLSCREEN;
    } else {
      videoFlags = videoFlags | SDL_WINDOW_RESIZABLE;
    }
  }
  
  _window = SDL_CreateWindow("Dagon", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED,
                             config.displayWidth, config.displayHeight,
                             videoFlags);
  _context = SDL_GL_CreateContext(_window);
  
  if (!_window) {
    log.error(kModSystem, "%s", kString13010);
    return false;
  }
  
  // Set vertical sync according to our configuration
  if (config.verticalSync) {
    SDL_GL_SetSwapInterval(1);
  }
  else {
    // Force our own frame limiter on
    config.frameLimiter = true;
    SDL_GL_SetSwapInterval(0);
  }
  
  SDL_WarpMouseInWindow(_window, config.displayWidth / 2,
                        config.displayHeight / 2);
  SDL_ShowCursor(false);
  
    return true;
}

void DGSystem::setTitle(const char* title) {
  SDL_SetWindowTitle(_window, title);
}

void DGSystem::update() {
  config.setFramesPerSecond(_calculateFrames(kFrameratePrecision));
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_KEYDOWN: {
        switch (event.key.keysym.sym) {
          case SDLK_F1:
          case SDLK_F2:
          case SDLK_F3:
          case SDLK_F4:
          case SDLK_F5:
          case SDLK_F6:
          case SDLK_F7:
          case SDLK_F8:
          case SDLK_F9:
          case SDLK_F10:
          case SDLK_F11:
          case SDLK_F12:
            DGControl::instance().processFunctionKey(event.key.keysym.sym);
            break;
          case SDLK_BACKSPACE:
          case SDLK_ESCAPE:
          case SDLK_TAB:
          case SDLK_RETURN:
            DGControl::instance().processKey(event.key.keysym.sym,
                                             DGEventKeyDown);
            break;
          case SDLK_f:
            SDL_Keymod modified = SDL_GetModState();
            if (modified & (KMOD_LALT | KMOD_LCTRL | KMOD_LGUI))                this->toggleFullscreen();
            break;
        }
        break;
      }
      case SDL_MOUSEMOTION:
        DGControl::instance().processMouse(event.motion.x,
                                           event.motion.y,
                                           DGEventMouseMove);
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          DGControl::instance().processMouse(event.button.x,
                                             event.button.y,
                                             DGEventMouseDown);
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          DGControl::instance().processMouse(event.button.x,
                                             event.button.y,
                                             DGEventMouseRightDown);
        }
        break;
      case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT) {
          DGControl::instance().processMouse(event.button.x,
                                             event.button.y,
                                             DGEventMouseUp);
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          DGControl::instance().processMouse(event.button.x,
                                             event.button.y,
                                             DGEventMouseRightUp);
        }
        break;
      case SDL_TEXTINPUT:
          DGControl::instance().processKey(event.text.text[0],
                                           DGEventKeyDown);
        break;
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
          case SDL_WINDOWEVENT_CLOSE:
            DGControl::instance().terminate();
            break;
          case SDL_WINDOWEVENT_LEAVE:
            DGControl::instance().processMouse(config.displayWidth / 2,
                                               config.displayHeight / 2,
                                               DGEventMouseMove);
            break;
          case SDL_WINDOWEVENT_RESIZED:
            DGControl::instance().reshape(event.window.data1,
                                          event.window.data2);
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  
  SDL_GL_SwapWindow(_window);
}

void DGSystem::terminate() {
  SDL_GL_DeleteContext(_context);
  if (config.fullscreen)
    SDL_SetWindowFullscreen(_window, 0);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

void DGSystem::toggleFullscreen() {
  config.fullscreen = !config.fullscreen;
  if (config.fullscreen) {
    SDL_DisplayMode desktopMode;
    SDL_GetDesktopDisplayMode(0, &desktopMode);
    SDL_SetWindowPosition(_window, 0, 0);
    SDL_SetWindowSize(_window, desktopMode.w, desktopMode.h);
    SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
  } else {
    SDL_SetWindowFullscreen(_window, 0);
  }
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

// TODO: For best precision, this should be suspended when
// performing a switch (the most expensive operation)
double DGSystem::_calculateFrames(double theInterval = 1.0) {
  static double lastTime = SDL_GetTicks() / 1000;
  static int fpsFrameCount = 0;
  static double fps = 0.0;
    
  double currentTime = SDL_GetTicks() / 1000;

  if (theInterval < 0.1)
    theInterval = 0.1;
    
  if (theInterval > 10.0)
    theInterval = 10.0;
    
  if ((currentTime - lastTime) > theInterval) {
    fps = (double)fpsFrameCount / (currentTime - lastTime);

    fpsFrameCount = 0;
    lastTime = SDL_GetTicks() / 1000;
  }
  else fpsFrameCount++;

  return fps;
}
