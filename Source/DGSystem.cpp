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
        log.info(kModSystem, "%s: %d.%d", kString13003, SFML_VERSION_MAJOR, SFML_VERSION_MINOR);
        
        if (!config.displayWidth || !config.displayHeight) {
            if (config.fullscreen) {
              config.displayWidth = sf::VideoMode::getDesktopMode().width;
              config.displayHeight = sf::VideoMode::getDesktopMode().height;
            }
            else {
              // Use a third of the screen
              config.displayWidth = sf::VideoMode::getDesktopMode().width / 1.5;
              config.displayHeight = sf::VideoMode::getDesktopMode().height / 1.5;
            }
        }
      
      if (config.fullscreen)
        _window.create(sf::VideoMode(config.displayWidth, config.displayHeight), "Dagon", sf::Style::Fullscreen);
        else
            _window.create(sf::VideoMode(config.displayWidth, config.displayHeight), "Dagon");
        
        // Set vertical sync according to our configuration
        if (config.verticalSync) {
            _window.setVerticalSyncEnabled(true);
        }
        else {
            // Force our own frame limiter on
            config.frameLimiter = true;
            _window.setVerticalSyncEnabled(false);
        }
      
            sf::Mouse::setPosition(sf::Vector2i(config.displayWidth / 2, config.displayHeight / 2), _window);
      _window.setMouseCursorVisible(false);
      
        _isInitialized = true;
    }
    else log.warning(kModSystem, "%s", kString13004);
    
    return true;
}

void DGSystem::setTitle(const char* title) {
  _window.setTitle(title);
}

void DGSystem::update() {
  config.setFramesPerSecond(_calculateFrames(kFrameratePrecision));
  if (_window.isOpen()) {
    _window.display();
    
    sf::Event event;
    while (_window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed: {
          _window.close();
          break;
        }
        case sf::Event::Resized: {
          DGControl::instance().reshape(event.size.width, event.size.height);
          break;
        }
        case sf::Event::LostFocus: {

          break;
        }
        case sf::Event::GainedFocus: {
          break;
        }
        case sf::Event::TextEntered: {
          if (DGControl::instance().isConsoleActive())
            DGControl::instance().processKey(event.text.unicode, DGEventKeyDown);
          break;
        }
        case sf::Event::KeyPressed: {
          if (event.key.alt)
            this->toggleFullscreen();
          else
            DGControl::instance().processKey(event.key.code, DGEventKeyDown);
          break;
        }
        case sf::Event::KeyReleased: {
          break;
        }
        case sf::Event::MouseWheelMoved: {
          break;
        }
        case sf::Event::MouseButtonPressed: {
          if (event.mouseButton.button == sf::Mouse::Left) {
            DGControl::instance().processMouse(event.mouseButton.x,
                                               event.mouseButton.y,
                                               DGEventMouseDown);
          } else if (event.mouseButton.button == sf::Mouse::Right) {
            DGControl::instance().processMouse(event.mouseButton.x,
                                               event.mouseButton.y,
                                               DGEventMouseRightDown);
          }
          break;
        }
        case sf::Event::MouseButtonReleased: {
          if (event.mouseButton.button == sf::Mouse::Left) {
            DGControl::instance().processMouse(event.mouseButton.x,
                                               event.mouseButton.y,
                                               DGEventMouseUp);
          } else if (event.mouseButton.button == sf::Mouse::Right) {
            DGControl::instance().processMouse(event.mouseButton.x,
                                               event.mouseButton.y,
                                               DGEventMouseRightUp);
          }
          break;
        }
        case sf::Event::MouseMoved: {
          DGControl::instance().processMouse(event.mouseMove.x,
                                             event.mouseMove.y,
                                             DGEventMouseMove);
          break;
        }
        case sf::Event::MouseEntered: {
          break;
        }
        case sf::Event::MouseLeft: {
          DGControl::instance().processMouse(config.displayWidth / 2,
                                            config.displayHeight / 2,
                                             DGEventMouseMove);
          break;
        }
        case sf::Event::JoystickButtonPressed: {
          break;
        }
        case sf::Event::JoystickButtonReleased: {
          break;
        }
        case sf::Event::JoystickMoved: {
          break;
        }
        case sf::Event::JoystickConnected: {
          break;
        }
        case sf::Event::JoystickDisconnected: {
          break;
        }
        case sf::Event::Count: {
          break;
        }
      }
    }
  }
}

void DGSystem::terminate() {
  _isInitialized = false;
  _window.close();
  exit(EXIT_SUCCESS);
}

void DGSystem::toggleFullscreen() {
  config.fullscreen = !config.fullscreen;
  
  _window.close();
  
  if (config.fullscreen)
    _window.create(sf::VideoMode(config.displayWidth, config.displayHeight), "Dagon", sf::Style::Fullscreen);
  else
    _window.create(sf::VideoMode(config.displayWidth, config.displayHeight), "Dagon");
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

// TODO: For best precision, this should be suspended when
// performing a switch (the most expensive operation)
double DGSystem::_calculateFrames(double theInterval = 1.0) {
  sf::Time time = _clock.getElapsedTime();
	static double lastTime = time.asSeconds();
	static int fpsFrameCount = 0;
	static double fps = 0.0;
    
	double currentTime = time.asSeconds();

	if (theInterval < 0.1)
		theInterval = 0.1;
    
	if (theInterval > 10.0)
		theInterval = 10.0;
    
	if ((currentTime - lastTime) > theInterval) {
		fps = (double)fpsFrameCount / (currentTime - lastTime);

		fpsFrameCount = 0;
		lastTime = time.asSeconds();
	}
	else fpsFrameCount++;

	return fps;
}
