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
// DISCLAIMER: This file is quite messy and requires a major
// overhaul, especially the processing of input.
// Proceed at your own risk.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "AudioManager.h"
#include "CameraManager.h"
#include "Config.h"
#include "Console.h"
#include "Control.h"
#include "CursorManager.h"
#include "FeedManager.h"
#include "FontManager.h"
#include "EffectsManager.h"
#include "Interface.h"
#include "InternalAudio.h"
#include "Log.h"
#include "Node.h"
#include "RenderManager.h"
#include "Room.h"
#include "Scene.h"
#include "Script.h"
#include "Spot.h"
#include "State.h"
#include "System.h"
#include "TextureManager.h"
#include "TimerManager.h"
#include "VideoManager.h"

#include <algorithm>
#include <iterator>
#include <set>

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Control::Control() :
audioManager(AudioManager::instance()),
cameraManager(CameraManager::instance()),
config(Config::instance()),
cursorManager(CursorManager::instance()),
feedManager(FeedManager::instance()),
fontManager(FontManager::instance()),
log(Log::instance()),
renderManager(RenderManager::instance()),
script(Script::instance()),
system(Config::instance(), Log::instance()),
textureManager(TextureManager::instance()),
timerManager(TimerManager::instance()),
videoManager(VideoManager::instance())
{

  _currentRoom = NULL;

  _sleepTimer = 0;

  _isInitialized = false;
  _isShowingSplash = false;
  _isShuttingDown = false;
  _isRunning = false;

  // This is used to randomize the color of certain spots,
  // should be called once during initialization.
  srand((unsigned)time(0));

  // In case the game is loaded in a slide, this should have a sensible default value.
  // Optionally this can be added to the saved state to preserve it between sessions.
  // But for now, this simpler solution is adequate.
  _prevControlMode = kControlFree;

  // For precaution, we set all these to false

  _eventHandlers.hasEnterNode = false;
  _eventHandlers.hasLeaveNode = false;
  _eventHandlers.hasEnterRoom = false;
  _eventHandlers.hasLeaveRoom = false;
  _eventHandlers.hasPreRender = false;
  _eventHandlers.hasPostRender = false;
  _eventHandlers.hasMouseButton = false;
  _eventHandlers.hasMouseRightButton = false;
  _eventHandlers.hasMouseMove = false;
  _eventHandlers.hasResize = false;

  for (int i = 0; i < kMaxHotKeys; i++)
    _hotkeyData[i].enabled = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

Control::~Control() {
  if (_isInitialized) {
    delete _console;
    delete _interface;
    delete _state;
    delete _scene;
  }
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void Control::init(int argc, char* argv[]) {
#ifndef DEBUG
  // First thing we do to log properly
  system.findPaths();
#endif

  // FIXME: Solve problem when script not found (console isn't shown)
  script.init();

  system.init();

  renderManager.init();
  renderManager.resetView(); // Test for errors

  cameraManager.init();
  cameraManager.setViewport(config.displayWidth, config.displayHeight);

  // Init the audio manager
  audioManager.init();

  // Init the font library
  fontManager.init();

  // Init the texture manager
  //textureManager.init(); // Preloader thread not working yet

  // Init the video manager
  videoManager.init();

  feedManager.init();

  _interface = new Interface;
  _scene = new Scene;

  _state = new State;

  timerManager.setSystem(&system);

  _console = new Console;
  if (config.debugMode) {
    // Console must be initialized after the Font Manager
    _console->init();
    _console->enable();
  }

  // If the splash screen is enabled, load its data and set the correct state
  if (config.showSplash) {
    _cancelSplash = false;
    _scene->loadSplash();
    _state->set(StateSplash);
    cursorManager.disable();
    renderManager.fadeInNextUpdate();
  }
  else renderManager.resetFade();

  // Create an empty Room for claiming assets that don't belong to other rooms.
  _arrayOfRooms.insert(_arrayOfRooms.begin(), new Room);

  _directControlActive = true;
  _isInitialized = true;

  // Force a redraw to avoid an annoying blink
  this->reshape(config.displayWidth, config.displayHeight);

  if (!config.showSplash)
    script.run();
  else
    _isShowingSplash = true;

  log.trace(kModControl, "%s", kString13002);
}

Node* Control::currentNode() {
  if (_currentRoom)
    return _currentRoom->currentNode();
  else
    return NULL;
}

Room* Control::currentRoom() {
  return _currentRoom;
}

Room* Control::assetRoom() const {
  return _arrayOfRooms[0];
}

void Control::cutscene(const char* fileName) {
  _scene->loadCutscene(fileName);
  _state->set(StateCutscene);
  cursorManager.fadeOut();
}

bool Control::isConsoleActive() {
  return !_console->isHidden();
}

bool Control::isDirectControlActive() {
  if (config.controlMode == kControlFixed)
    return _directControlActive;
  else
    return false;
}

void Control::lookAt(float horizontal, float vertical, bool instant, bool adjustment) {
  if (instant) {
    cameraManager.setAngleHorizontal(horizontal);
    cameraManager.setAngleVertical(vertical);
  }
  else {
    cameraManager.setTargetAngle(horizontal, vertical, adjustment);
    _state->set(StateLookAt);
    cursorManager.fadeOut();
  }
}

void Control::processFunctionKey(int aKey) {
  int idx = 0;

  switch (aKey) {
    case kKeyF1: idx = 1; break;
    case kKeyF2: idx = 2; break;
    case kKeyF3: idx = 3; break;
    case kKeyF4: idx = 4; break;
    case kKeyF5: idx = 5; break;
    case kKeyF6: idx = 6; break;
    case kKeyF7: idx = 7; break;
    case kKeyF8: idx = 8; break;
    case kKeyF9: idx = 9; break;
    case kKeyF10: idx = 10; break;
    case kKeyF11: idx = 11; break;
    case kKeyF12: idx = 12; break;
  }

  if (idx) {
    if (_hotkeyData[idx].enabled)
      script.processCommand(_hotkeyData[idx].line);
  }
}

void Control::processKey(int aKey, int eventFlags) {
  int idx = 0;

  switch (eventFlags) {
    case EventKeyDown:
      switch (aKey) {
        case kKeyEscape:
          if (_hotkeyData[18].enabled && _console->isHidden()) { // 18 is escape's index. Is it assigned as a hotkey?
            script.processCommand(_hotkeyData[18].line);
          }
          else if (_state->current() == StateSplash) {
            _cancelSplash = true;
          }
          else {
            if (feedManager.isPlaying())
              feedManager.cancel();
            else if (_state->current() == StateCutscene) {
              _scene->cancelCutscene();
            }
          }
          break;
        case kKeyTab:
          if (config.debugMode) _console->toggle();
          break;
        case kKeySpace:
          if (_hotkeyData[19].enabled && _console->isHidden()) { // 19 is space's index. Is it assigned as a hotkey?
            script.processCommand(_hotkeyData[19].line);
          }
          break;

        case kKeyI: idx = 13; break;
        case kKeyJ: idx = 14; break;
        case kKeyL: idx = 15; break;
        case kKeyO: idx = 16; break;
        case kKeyP: idx = 17; break;
      }

      if (idx != 0 && _console->isHidden()) {
        if (_hotkeyData[idx].enabled) {
          script.processCommand(_hotkeyData[idx].line);
        }
      }

      // Process these keys only when the console is visible
      if (!_console->isHidden()) {
        switch (aKey) {
          case kKeyEscape:
          case kKeyTab:
            // Ignore these
            break;
          case kKeyBackspace:
            _console->deleteChar();
            break;
          case kKeyEnter:
            _console->execute();
            break;
          default:
            _console->inputChar(aKey);
            break;
        }
      }
      break;

    case EventKeyModified:
      switch (aKey) {
        case 'f':
        case 'F':
          config.fullscreen = !config.fullscreen;
          system.toggleFullscreen();
          break;
      }
      break;
  }
}

void Control::processMouse(int x, int y, int eventFlags) {
  // TODO: Horrible nesting of IFs here... improve

  if ((config.controlMode != kControlFixed) ||
      !_directControlActive) {
    cursorManager.updateCoords(x, y);
  }

  if (!cursorManager.isEnabled() || cursorManager.isFading()) {
    // Ignore all the rest
    cursorManager.setCursor(kCursorNormal);
    cameraManager.stopPanning();
    return;
  }

  if ((eventFlags == EventMouseMove) && (_eventHandlers.hasMouseMove))
    script.processCallback(_eventHandlers.mouseMove, 0);

  if ((eventFlags == EventMouseUp) && _eventHandlers.hasMouseButton) {
    script.processCallback(_eventHandlers.mouseButton, 0);
  }

  if ((eventFlags == EventMouseRightUp) && _eventHandlers.hasMouseRightButton) {
    script.processCallback(_eventHandlers.mouseRightButton, 0);
  }

  // The overlay system is handled differently than the spots, so we
  // do everything here. Eventually, we should tide up things a little.

  bool canProcess = false;

  if (config.controlMode == kControlFixed) {
    if (!_directControlActive)
      canProcess = true;
  }
  else {
    if (!cursorManager.isDragging())
      canProcess = true;
  }

  // TODO: Use active overlays only
  if (canProcess) {
    if (_interface->scanOverlays()) {
      if ((eventFlags == EventMouseUp) && cursorManager.hasAction()) {
        _processAction();

        // Repeat the scan in case the button is no longer visible
        _interface->scanOverlays();

        // Stop processing spots
        return;
      }
    }
  }

  switch (config.controlMode) {
    case kControlFree:
      if (eventFlags == EventMouseMove) {
        if (!cursorManager.onButton()) {
          cameraManager.pan(x, y);

          // When in free mode, we first check if camera is panning because
          // we don't want the "not dragging" cursor
          if (cameraManager.isPanning() && !cursorManager.hasAction()) {
            cursorManager.setCursor(cameraManager.cursorWhenPanning());
          }
          else cursorManager.setCursor(kCursorNormal);

        }
        else cameraManager.stopPanning();
      }

      if (eventFlags == EventMouseUp) {
        if (cursorManager.hasAction())
          _processAction();
      }
      break;

    case kControlFixed:
      if (_directControlActive) {
        cursorManager.updateCoords(config.displayWidth >> 1, config.displayHeight >> 1); // Forced

        if (eventFlags == EventMouseMove) {
          cameraManager.setDirectPan(x, y);
        }
      }

      if (eventFlags == EventMouseUp) {
        if (cursorManager.hasAction()) {
          _processAction();
        }
      }
      break;
  }
}

void Control::registerGlobalHandler(int forEvent, int handlerForLua) {
  switch (forEvent) {
    case EventEnterNode:
      _eventHandlers.enterNode = handlerForLua;
      _eventHandlers.hasEnterNode = true;
      break;
    case EventLeaveNode:
      _eventHandlers.leaveNode = handlerForLua;
      _eventHandlers.hasLeaveNode = true;
      break;
    case EventEnterRoom:
      _eventHandlers.enterRoom = handlerForLua;
      _eventHandlers.hasEnterRoom = true;
      break;
    case EventLeaveRoom:
      _eventHandlers.leaveRoom = handlerForLua;
      _eventHandlers.hasLeaveRoom = true;
      break;
    case EventPreRender:
      _eventHandlers.preRender = handlerForLua;
      _eventHandlers.hasPreRender = true;
      break;
    case EventPostRender:
      _eventHandlers.postRender = handlerForLua;
      _eventHandlers.hasPostRender = true;
      break;
    case EventMouseDown:
      _eventHandlers.mouseButton = handlerForLua;
      _eventHandlers.hasMouseButton = true;
      break;
    case EventMouseRightDown:
      _eventHandlers.mouseRightButton = handlerForLua;
      _eventHandlers.hasMouseRightButton = true;
      break;
    case EventMouseMove:
      _eventHandlers.mouseMove = handlerForLua;
      _eventHandlers.hasMouseMove = true;
      break;
    case EventResize:
      _eventHandlers.resize = handlerForLua;
      _eventHandlers.hasResize = true;
      break;
  }
}

void Control::registerHotkey(int aKey, const char* luaCommandToExecute) {
  int idx = 0;

  switch (aKey) {
    case kKeyF1: idx = 1; break;
    case kKeyF2: idx = 2; break;
    case kKeyF3: idx = 3; break;
    case kKeyF4: idx = 4; break;
    case kKeyF5: idx = 5; break;
    case kKeyF6: idx = 6; break;
    case kKeyF7: idx = 7; break;
    case kKeyF8: idx = 8; break;
    case kKeyF9: idx = 9; break;
    case kKeyF10: idx = 10; break;
    case kKeyF11: idx = 11; break;
    case kKeyF12: idx = 12; break;
    case kKeyI: idx = 13; break;
    case kKeyJ: idx = 14; break;
    case kKeyL: idx = 15; break;
    case kKeyO: idx = 16; break;
    case kKeyP: idx = 17; break;
    case kKeyEscape: idx = 18; break;
    case kKeySpace: idx = 19; break;
  }

  if (idx) {
    _hotkeyData[idx].enabled = true;
    _hotkeyData[idx].value = aKey;
    strncpy(_hotkeyData[idx].line, luaCommandToExecute, kMaxLogLength);
  }
}

void Control::registerObject(Object* theTarget) {
  switch (theTarget->type()) {
    case kObjectNode:
      textureManager.requestBundle((Node*)theTarget);
      break;
    case kObjectOverlay:
      _interface->addOverlay((Overlay*)theTarget);
      break;
    case kObjectRoom:
      _arrayOfRooms.push_back((Room*)theTarget);
      break;
    case kObjectVideo:
      videoManager.registerVideo((Video*)theTarget);
      break;
  }

  if (_isRunning && !_isShowingSplash)
    log.warning(kModControl, "%s: %s", kString12007, theTarget->name().c_str());
}

void Control::reshape(int width, int height) {
  // TODO: Must store the initial width value
  int size = (width * kDefCursorSize) / 1920;

  if (size > kMaxCursorSize)
    size = kMaxCursorSize;
  else if (size < DGMinCursorSize)
    size = DGMinCursorSize;

  config.displayWidth = width;
  config.displayHeight = height;

  cameraManager.setViewport(width, height);
  cursorManager.setSize(size);
  feedManager.reshape();
  renderManager.reshape();

  if (_eventHandlers.hasResize)
    script.processCallback(_eventHandlers.resize, 0);
}

void Control::run() {
  _isRunning = true;

  double startTime = SDL_GetTicks();
  double updateInterval = (1.0 / (double)config.framerate) * 1000;

  while (_isRunning) {
    if (config.frameLimiter) {
      while (startTime < SDL_GetTicks()) {
        this->update();
        startTime += updateInterval;
      }
    }
    else {
      this->update();
    }
  }
}

void Control::sleep(int forSeconds) {
  _sleepTimer = timerManager.createManual(forSeconds);
  _state->set(StateSleep);
  cursorManager.fadeOut();
}

void Control::switchTo(Object* theTarget) {
  static bool firstSwitch = true;
  static SettingCollection previousEffects; // Temporary patch

  //log.trace(kModControl, "Begin switching...");

  _updateView(StateNode, true);

  videoManager.flush();

  if (firstSwitch) {
    firstSwitch = false;
  }
  else {
    renderManager.blendNextUpdate(true);
  }

  cameraManager.setViewport(config.displayWidth, config.displayHeight);

  if (currentNode()){
    if (currentNode()->hasLeaveEvent()) {
      //log.trace(kModControl, "Has node onLeave callback");
      script.processCallback(currentNode()->leaveEvent(), 0);
    }
  }

  if (theTarget) {
    switch (theTarget->type()) {
      case kObjectRoom: {
        //log.trace(kModControl, "Switching to room...");
        feedManager.cancel();

        if (currentNode()) {
          if (currentNode()->isSlide())
            cameraManager.unlock();
        }

        static_cast<Room*>(theTarget)->claimAssets();
        if (_currentRoom) {
          _currentRoom->releaseAssets(static_cast<Room*>(theTarget));
        }

        _currentRoom = (Room*)theTarget;
        //log.trace(kModControl, "Set room and Lua objet");
        _scene->setRoom((Room*)theTarget);
        timerManager.setLuaObject(_currentRoom->luaObject());

        if (!_currentRoom->hasNodes()) {
          log.warning(kModControl, "%s: %s", kString12005, _currentRoom->name().c_str());
          return;
        }

        // This has to be done every time so that room audios keep playing
        for (auto audio : _currentRoom->arrayOfAudios()) {
          if (!audio->isPlaying() || audio->isFading()) {
            audio->fadeIn();
          }

          if (audio->doesAutoplay()) {
            audioManager.registerAudio(audio);
            audio->play();
          }
        }

        textureManager.setRoomToPreload(_currentRoom);

        if (_eventHandlers.hasEnterRoom) {
          //log.trace(kModControl, "Has global enter event");
          script.processCallback(_eventHandlers.enterRoom, 0);
        }

        if (_currentRoom->hasEnterEvent()) {
          //log.trace(kModControl, "Has room enter event");
          script.processCallback(_currentRoom->enterEvent(), 0);
        }

        EffectsManager& effectsManager = EffectsManager::instance();
        if (_currentRoom->hasEffects()) {
          //log.trace(kModControl, "Loading room effects");
          effectsManager.saveSettings(&previousEffects);
          effectsManager.loadSettings(_currentRoom->effects());
        } else {
          if (!previousEffects.empty()) {
            //log.trace(kModControl, "Loading previous effects");
            effectsManager.loadSettings(previousEffects);
          }
        }
        break;
      }
      case kObjectSlide:
        //log.trace(kModControl, "Switching to slide...");
        if (_currentRoom) {
          if (currentNode() && !currentNode()->isSlide()) {
            // We are switching to a slide from something that is not a slide.
            // The control mode should be saved.
            _prevControlMode = config.controlMode;
          }

          Node* node = (Node*)theTarget;
          //log.trace(kModControl, "Set previous node");
          node->setPreviousNode(this->currentNode());

          if (!_currentRoom->switchTo(node)) {
            log.error(kModControl, "%s: %s (%s)", kString12010, node->name().c_str(), _currentRoom->name().c_str()); // Bad slide
            return;
          }

          _directControlActive = false;

          cameraManager.lock();
        }
        else {
          log.error(kModControl, "%s", kString12009);
          return;
        }

        break;
      case kObjectNode: {
        //log.trace(kModControl, "Switching to node...");
        if (currentNode()->isSlide())
          cameraManager.unlock();

        //log.trace(kModControl, "Set parent room");
        Node* curNode = currentNode();
        std::set<Audio*> curNodeAudios;

        if (curNode && curNode->hasSpots()) {
          curNode->beginIteratingSpots();

          do {
            Spot *curSpot = curNode->currentSpot();
            if (curSpot->hasAudio() && curSpot->hasFlag(kSpotAuto)) {
              curNodeAudios.insert(curSpot->audio());
            }
          } while (curNode->iterateSpots());
        }

        Node* node = (Node*)theTarget;
        std::set<Audio*> targetNodeAudios;

        if (node->hasSpots()) {
          node->beginIteratingSpots();

          do {
            Spot *targetspot = node->currentSpot();
            if (targetspot->hasAudio() && targetspot->hasFlag(kSpotAuto)) {
              targetNodeAudios.insert(targetspot->audio());
            }
          } while (node->iterateSpots());
        }

        std::vector<Audio*> audiosToStop;
        std::set_difference(curNodeAudios.begin(), curNodeAudios.end(), targetNodeAudios.begin(),
                            targetNodeAudios.end(), std::back_inserter(audiosToStop));

        for (Audio *audio : audiosToStop) {
          if (audio->isPlaying()) {
            audio->fadeOut();
          }
        }

        std::vector<Audio*> audiosToPlay;
        std::set_difference(targetNodeAudios.begin(), targetNodeAudios.end(), curNodeAudios.begin(),
                            curNodeAudios.end(), std::back_inserter(audiosToPlay));

        for (Audio *audio : audiosToPlay) {
          if (!audio->isPlaying()) {
            audioManager.registerAudio(audio);
            audio->fadeIn();
            audio->play();
          }
        }

        Room* room = node->parentRoom();

        if (room) {
          if (room != _currentRoom) {
            //log.trace(kModControl, "New room, so cleaning up...");
            feedManager.cancel();
            _currentRoom = room;
            _scene->setRoom(room);
            timerManager.setLuaObject(_currentRoom->luaObject());
            textureManager.setRoomToPreload(_currentRoom);

            if (_eventHandlers.hasEnterRoom) {
              //log.trace(kModControl, "Has global room enter event");
              script.processCallback(_eventHandlers.enterRoom, 0);
            }

            if (_currentRoom->hasEnterEvent()) {
              //log.trace(kModControl, "Has room enter event");
              script.processCallback(_currentRoom->enterEvent(), 0);
            }

            EffectsManager& effectsManager = EffectsManager::instance();
            if (_currentRoom->hasEffects()) {
              //log.trace(kModControl, "Loading room effects");
              effectsManager.saveSettings(&previousEffects);
              effectsManager.loadSettings(_currentRoom->effects());
            } else {
              if (!previousEffects.empty()) {
                //log.trace(kModControl, "Loading previous effects");
                effectsManager.loadSettings(previousEffects);
              }
            }
          }
        }
        else {
          log.error(kModControl, "%s", kString12009);
          return;
        }

        if (!_currentRoom->switchTo(node)) {
          log.error(kModControl, "%s: %s (%s)", kString12008, node->name().c_str(), _currentRoom->name().c_str()); // Bad node
          return;
        }

        if (_eventHandlers.hasEnterNode) {
          //log.trace(kModControl, "Has global node enter event");
          script.processCallback(_eventHandlers.enterNode, 0);
        }

        if (node->hasEnterEvent()) {
          //log.trace(kModControl, "Has node enter event");
          script.processCallback(node->enterEvent(), 0);
        }

        break;
      }
    }
  }
  else {
    // Only slides switch to NULL targets, so we check whether the new object is another slide.
    // If it isn't, we unlock the camera.
    if (_currentRoom) {
      //log.trace(kModControl, "Returning from slide...");
      Node* current = this->currentNode();
      Node* previous = current->previousNode();

      _currentRoom->switchTo(previous);

      if (current->slideReturn()) {
        //log.trace(kModControl, "Has slide onLeave callback");
        script.processCallback(current->slideReturn(), current->luaObject());
      }

      if (!previous->isSlide()) {
        //log.trace(kModControl, "Unlock camera and return to direct control");
        config.controlMode = _prevControlMode;
        cameraManager.setViewport(config.displayWidth, config.displayHeight);
        cameraManager.unlock();
        if (config.controlMode == kControlFixed)
          // FIXME: Forced, but it should return to the previous mode
          _directControlActive = true;
      }
    }
  }

  if (_currentRoom) {
    if (_currentRoom->hasNodes()) {
      // Now we proceed to load the textures of the current node
      Node* current = _currentRoom->currentNode();
      //log.trace(kModControl, "Flushing textures...");
      textureManager.flush();

      if (current->hasSpots()) {
        current->beginIteratingSpots();
        //log.trace(kModControl, "Processing spots...");
        do {
          Spot* spot = current->currentSpot();

          if (spot->hasAudio()) {
            //log.trace(kModControl, "Loading audio...");
            Audio* audio = spot->audio();

            // Ignore handling fade if audio is set to 0 at this stage
            if (audio->fadeLevel() > 0) {
              audio->setDefaultFadeLevel(spot->volume());
            }

            audio->setPosition(spot->face(), spot->origin());
          }

          // TODO: Merge the video autoplay with spot properties
          // TODO: Decide after video finishes playing if last frame is showed or removed
          if (spot->hasVideo()) {
            //log.trace(kModControl, "Loading video...");
            Video* video = spot->video();
            videoManager.requestVideo(video);

            if (video->isLoaded()) {
              if (!spot->hasTexture()) {
                Texture* texture = new Texture;
                spot->setTexture(texture);
              }

              video->play();

              DGFrame* frame = video->currentFrame();
              spot->texture()->loadRawData(frame->data, frame->width, frame->height);

              video->pause();
            }
          }

          if (spot->hasTexture()) {
            //log.trace(kModControl, "Loading image...");
            textureManager.requestTexture(spot->texture());

            // Only resize if nothing but origin
            if (spot->vertexCount() == 1)
              spot->resize(spot->texture()->width(), spot->texture()->height());
          }

          if (spot->hasFlag(kSpotAuto) || spot->isPlaying())
            spot->play();
        } while (current->iterateSpots());
      }
      else {
        log.warning(kModControl, "%s", kString12006);
      }

      // Prepare the name for the window
      char title[kMaxObjectName];
      snprintf(title, kMaxObjectName, "%s (%s, %s)", config.script().c_str(),
               _currentRoom->name().c_str(), current->description().c_str());
      //log.trace(kModControl, "Set window title");
      system.setTitle(title);
    }
  }

  //log.trace(kModControl, "Flushing audio...");

  cameraManager.stopPanning();

  //log.trace(kModControl, "Done!");
}

void Control::walkTo(Object* theTarget) {
  this->switchTo(theTarget);

  cameraManager.simulateWalk();
  Node* current = this->currentNode();

  // Finally, check if must play a single footstep
  if (current->hasFootstep()) {
    assetRoom()->claimAsset(current->footstep());
    audioManager.registerAudio(current->footstep());
    current->footstep()->play();

    Audio* footstep = new InternalAudio(true);
    footstep->setAudioName(current->footstep()->audioName());
    current->setFootstep(footstep);
  }
  else if (_currentRoom->hasDefaultFootstep()) {
    assetRoom()->claimAsset(_currentRoom->defaultFootstep());
    audioManager.registerAudio(_currentRoom->defaultFootstep());
    _currentRoom->defaultFootstep()->play();

    Audio* footstep = new InternalAudio(true);
    footstep->setAudioName(_currentRoom->defaultFootstep()->audioName());
    _currentRoom->setDefaultFootstep(footstep);
  }
}

void Control::syncSpot(Spot* spot) {
  _syncedSpot = spot;
  _state->set(StateVideoSync);
  cursorManager.fadeOut();
}

void Control::takeSnapshot() {
  bool previousTexCompression = config.texCompression;
  config.texCompression = false;

  time_t rawtime;
  struct tm* timeinfo;
  char buffer[kMaxFileLength];
  Texture texture(0, 0, 0);

  time(&rawtime);
  timeinfo = localtime (&rawtime);

  strftime(buffer, kMaxFileLength, "snap-%Y-%m-%d-%Hh%Mm%Ss", timeinfo);

  cameraManager.beginOrthoView();
  renderManager.drawPostprocessedView();
  texture.bind();
  renderManager.copyView();
  cameraManager.endOrthoView();

  texture.saveToFile(buffer);

  config.texCompression = previousTexCompression;
}

void Control::initiateTerminate() {
  if (!_isShuttingDown) {
    _scene->fadeOut();
    _shutdownTimer = timerManager.createManual(1.0);
    _isShuttingDown = true;
  }
}

void Control::terminate() {
  _isRunning = false;

  audioManager.terminate();
  timerManager.terminate();
  videoManager.terminate();

  assetRoom()->releaseAssets(nullptr);

  int r = rand() % 8; // Double the replies, so that the default one appears often

  switch (r) {
    default:
    case 0: log.trace(kModControl, "%s", kString12100); break;
    case 1: log.trace(kModControl, "%s", kString12101); break;
    case 2: log.trace(kModControl, "%s", kString12102); break;
    case 3: log.trace(kModControl, "%s", kString12103); break;
  }

  system.terminate();
}

void Control::update() {
  switch (_state->current()) {
    case StateLookAt:
      cameraManager.panToTargetAngle();
      _updateView(_state->previous(), false);

      if (!cameraManager.isPanning()) {
        _state->setPrevious();
        cursorManager.fadeIn();
        script.resume();
      }
      break;
    case StateSleep:
      if (timerManager.checkManual(_sleepTimer)) {
        _state->setPrevious();
        cursorManager.fadeIn();
        script.resume();
      }
      else {
        _updateView(_state->previous(), false);
      }

      break;
    case StateVideoSync:
      _updateView(_state->previous(), false);

      if (!_syncedSpot->video()->isPlaying()) {
        _state->setPrevious();
        cursorManager.fadeIn();
        script.resume();
      }
      break;
    default:
      _updateView(_state->current(), false);
      break;
  }

  if (_isShuttingDown) {
    if (timerManager.checkManual(_shutdownTimer)) {
      this->terminate();
    }
  }
}

std::vector<Room*> Control::rooms() {
  return _arrayOfRooms;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void Control::_processAction() {
  Action* action = cursorManager.action();

  switch (action->type) {
    case kActionFunction:
      script.processCallback(action->luaHandler, action->luaObject);
      break;
    case kActionFeed:
      if (action->hasFeedAudio) {
        feedManager.showAndPlay(action->feed.c_str(), action->feedAudio.c_str());
      }
      else feedManager.show(action->feed.c_str());
      break;
    case kActionSwitch:
      // FIXME: Temporary hack until we separate the slide code from node code
      if (this->currentNode() != NULL) {
        if (this->currentNode()->isSlide()) {
          config.controlMode = kControlFixed;
          switchTo(action->target);
        }
        else
          walkTo(action->target);
      }
      break;
  }
  cursorManager.removeAction();
}

void Control::_updateView(int state, bool inBackground) {
  // TODO: Suspend all operations when doing a switch
  // FIXME: Add a render stack of Objects, especially for overlays
  // IMPORTANT: Ensure this function is thread-safe when switching rooms or nodes

  if (!inBackground) {
    // User post-render operations, supporting textures
    if (_eventHandlers.hasPreRender)
      script.processCallback(_eventHandlers.preRender, 0);
  }

  // Setup the scene

  _scene->clear();

  // Do this in a viewtimer for the frame limitter feature
  switch (state) {
    case StateCutscene:
      if (!_scene->drawCutscene()) {
        _scene->unloadCutscene();
        _state->setPrevious();
        cursorManager.fadeIn();
        script.resume();
      }
      break;
    case StateNode:
      _scene->scanSpots();
      _scene->drawSpots(inBackground);

      if (!inBackground) {
        _interface->drawHelpers();
        _interface->drawOverlays();
        feedManager.update();
        _interface->drawCursor();
      }

      break;
    case StateSplash:
      static int handlerIn = timerManager.createManual(4);
      static int handlerOut = timerManager.createManual(5);

      _scene->drawSplash();

      if (timerManager.checkManual(handlerIn)) {
        _scene->fadeOut();
      }

      if (timerManager.checkManual(handlerOut) || _cancelSplash) {
        cursorManager.enable();
        renderManager.clearView();
        renderManager.resetFade();
        _state->set(StateNode);
        _scene->unloadSplash();

        script.run();

        _isShowingSplash = false;
      }

      break;
  }

  if (!inBackground) {
    // User post-render operations, supporting textures
    if (_eventHandlers.hasPostRender)
      script.processCallback(_eventHandlers.postRender, 0);
  }

  // General fade, affects every graphic on screen
  renderManager.fadeView();

  // Debug info (if enabled)
  if (_console->isEnabled() && !inBackground) {
    // BUG: This causes a crash sometimes. Why?
    _console->update();

    // We do this here in case the command changes the viewport
    cameraManager.endOrthoView();

    if (_console->isReadyToProcess()) {
      char command[kMaxLogLength];
      _console->getCommand(command);
      script.processCommand(command);
    }
  }
  else cameraManager.endOrthoView();

  audioManager.setOrientation(cameraManager.orientation());

  timerManager.process();

  if (!inBackground) {
    // Flush the buffers
    system.update();
  }
}

}
