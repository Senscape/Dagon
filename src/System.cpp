////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
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
#include "Control.h"
#include "Log.h"
#include "System.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void System::browse(const char* url) {
  // TODO: Re-implement
  log.warning(kModSystem, "Browsing is currently disabled");
}

#ifdef DAGON_MAC
namespace {
#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
}
void System::findPaths() {
  CFURLRef mainRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
  CFURLRef parentRef = CFURLCreateCopyDeletingLastPathComponent(NULL, mainRef);
  CFStringRef macPath = CFURLCopyFileSystemPath(parentRef, kCFURLPOSIXPathStyle);
  
  const char *pathPtr = CFStringGetCStringPtr(macPath,
                                              CFStringGetSystemEncoding());
  chdir(pathPtr);
}
#else
void System::findPaths() {
  // Nothing to do for now
}
#endif

bool System::init() {
  log.trace(kModSystem, "%s", kString13001);
  SDL_version version;
  SDL_GetVersion(&version);
  log.info(kModSystem, "%s: %d.%d", kString13003,
           version.major, version.minor);
  
  SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER);
  Uint32 videoFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  bool forcedRes = true;
  
  if (!config.displayWidth || !config.displayHeight) {
    // Use a third of the screen
    SDL_DisplayMode desktopMode;
    SDL_GetDesktopDisplayMode(0, &desktopMode);
    config.displayWidth = static_cast<int>(desktopMode.w / 1.5f);
    config.displayHeight = static_cast<int>(desktopMode.h / 1.5f);
    forcedRes = false;
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

  if (config.fullscreen) {
    int width = config.displayWidth;
    int height = config.displayHeight;

    if (!forcedRes) {
      SDL_DisplayMode desktopMode;
      SDL_GetDesktopDisplayMode(0, &desktopMode);
      width = desktopMode.w;
      height = desktopMode.h;
    }

    SDL_SetWindowPosition(_window, 0, 0);
    SDL_SetWindowSize(_window, width, height);
    SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
  }
  
  // Set vertical sync according to our configuration
  SDL_GL_SetSwapInterval(config.verticalSync);
  
  // Force our own frame limiter on if vertical sync wasn't enabled
  if (!SDL_GL_GetSwapInterval())
    config.frameLimiter = true;
  
  SDL_WarpMouseInWindow(_window, config.displayWidth >> 1,
                        config.displayHeight >> 1);
  SDL_ShowCursor(false);
  
  return true;
}

void System::setTitle(const char* title) {
  SDL_SetWindowTitle(_window, title);
}

void System::update() {
  config.setFramesPerSecond(_calculateFrames(kFrameratePrecision));
  SDL_GL_SwapWindow(_window);
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
            Control::instance().processFunctionKey(event.key.keysym.sym);
            break;
          case SDLK_BACKSPACE:
          case SDLK_ESCAPE:
          case SDLK_TAB:
          case SDLK_RETURN:
            Control::instance().processKey(event.key.keysym.sym,
                                           EventKeyDown);
            break;
          case SDLK_f:
            SDL_Keymod modified = SDL_GetModState();
            if (modified & (KMOD_LALT | KMOD_LCTRL | KMOD_LGUI))
              this->toggleFullscreen();
            break;
        }
        break;
      }
        /*case SDL_KEYUP: {
         switch (event.key.keysym.sym) {
         case SDLK_w:
         case SDLK_a:
         case SDLK_s:
         case SDLK_d:
         Control::instance().processKey(event.key.keysym.sym,
         EventKeyUp);
         break;
         }
         }*/
      case SDL_MOUSEMOTION:
        Control::instance().processMouse(event.motion.x,
                                         event.motion.y,
                                         EventMouseMove);
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          Control::instance().processMouse(event.button.x,
                                           event.button.y,
                                           EventMouseDown);
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          Control::instance().processMouse(event.button.x,
                                           event.button.y,
                                           EventMouseRightDown);
        }
        break;
      case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT) {
          Control::instance().processMouse(event.button.x,
                                           event.button.y,
                                           EventMouseUp);
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          Control::instance().processMouse(event.button.x,
                                           event.button.y,
                                           EventMouseRightUp);
        }
        break;
      case SDL_TEXTINPUT:
        Control::instance().processKey(event.text.text[0],
                                       EventKeyDown);
        break;
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
          case SDL_WINDOWEVENT_CLOSE:
            Control::instance().terminate();
            break;
          case SDL_WINDOWEVENT_LEAVE:
            Control::instance().processMouse(config.displayWidth >> 1,
                                             config.displayHeight >> 1,
                                             EventMouseMove);
            break;
          case SDL_WINDOWEVENT_RESIZED:
            int w, h;
            SDL_GetWindowSize(_window, &w, &h);
            Control::instance().reshape(w, h);
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  if (config.controlMode == kControlFixed) {
    if (Control::instance().isDirectControlActive()) {
      double currentTime = SDL_GetTicks();
      static double lastTime = currentTime;
      if ((currentTime - lastTime) >= 100) {
        SDL_WarpMouseInWindow(_window, config.displayWidth >> 1,
                              config.displayHeight >> 1);
        lastTime = SDL_GetTicks();
      }
    }
  }
}

void System::terminate() {
  SDL_GL_DeleteContext(_context);
  if (config.fullscreen)
    SDL_SetWindowFullscreen(_window, 0);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

void System::toggleFullscreen() {
  config.fullscreen = !config.fullscreen;
  if (config.fullscreen) {
    SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  } else {
    SDL_SetWindowFullscreen(_window, 0);
    SDL_DisplayMode desktopMode;
    SDL_GetDesktopDisplayMode(0, &desktopMode);
    SDL_SetWindowPosition(_window, 10, 10);
    SDL_SetWindowSize(_window, desktopMode.w, desktopMode.h);
  }
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

// TODO: For best precision, this should be suspended when
// performing a switch (the most expensive operation)
double System::_calculateFrames(double theInterval = 1.0) {
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

}
