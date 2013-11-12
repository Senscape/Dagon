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
#include "Interface.h"
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

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Control::Control() :
audioManager(AudioManager::instance()),
textureManager(TextureManager::instance()),
cameraManager(CameraManager::instance()),
config(Config::instance()),
cursorManager(CursorManager::instance()),
feedManager(FeedManager::instance()),
fontManager(FontManager::instance()),
log(Log::instance()),
renderManager(RenderManager::instance()),
script(Script::instance()),
system(Config::instance(), Log::instance()),
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
  
  log.trace(kModControl, "========================================");
  log.info(kModControl, "%s: %s", kString12001, DAGON_VERSION_STRING);
  log.info(kModControl, "%s: %d", kString12004, DAGON_BUILD);
  
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
  switch (eventFlags) {
    case EventKeyDown:
      switch (aKey) {
        case kKeyEscape:
          if (_state->current() == StateSplash) {
            _cancelSplash = true;
          }
          else {
            if (feedManager.isPlaying())
              feedManager.cancel();
            else if (_state->current() == StateCutscene) {
              _scene->cancelCutscene();
            }
            else {
              if (!_isShuttingDown) {
                _scene->fadeOut();
                _shutdownTimer = timerManager.createManual(1.5);
                _isShuttingDown = true;
              }
            }
          }
          break;
        case kKeyQuote:
        case kKeyTab:
          _console->toggle();
          break;
        case kKeySpace:
          if (_console->isHidden())
            config.showHelpers = !config.showHelpers;
          break;
        case 'w':
        case 'W':
          if (_console->isHidden()) {
            cameraManager.pan(kCurrent, 0);
          }
          break;
        case 'a':
        case 'A':
          if (_console->isHidden()) {
            cameraManager.pan(0, kCurrent);
          }
          break;
        case 's':
        case 'S':
          if (_console->isHidden()) {
            cameraManager.pan(kCurrent, config.displayHeight);
          }
          break;
        case 'd':
        case 'D':
          if (_console->isHidden()) {
            cameraManager.pan(config.displayWidth, kCurrent);
          }
          break;
      }
      
      // Process these keys only when the console is visible
      if (!_console->isHidden()) {
        switch (aKey) {
          case kKeyEscape:
          case kKeyQuote:
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
      
    case EventKeyUp:
      if (_console->isHidden()) {
        switch (aKey) {
          case 'w':
          case 'W':
            cameraManager.pan(kCurrent, config.displayHeight >> 1);
            break;
          case 'a':
          case 'A':
            cameraManager.pan(config.displayWidth >> 1, kCurrent);
            break;
          case 's':
          case 'S':
            cameraManager.pan(kCurrent, config.displayHeight >> 1);
            break;
          case 'd':
          case 'D':
            cameraManager.pan(config.displayWidth >> 1, kCurrent);
            break;
        }
      }
      break;
  }
}

void Control::processMouse(int x, int y, int eventFlags) {
  // TODO: Horrible nesting of IFs here... improve
  
  if (config.controlMode == kControlFixed) {
    if (!_directControlActive) {
      cursorManager.updateCoords(x, y);
    }
  }
  else cursorManager.updateCoords(x, y);
  
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
          cameraManager.directPan(x, y);
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
  }
  
  if (idx) {
    _hotkeyData[idx].enabled = true;
    _hotkeyData[idx].value = aKey;
    strncpy(_hotkeyData[idx].line, luaCommandToExecute, kMaxLogLength);
  }
}

void Control::registerObject(Object* theTarget) {
  switch (theTarget->type()) {
    case kObjectAudio:
      audioManager.registerAudio((Audio*)theTarget);
      break;
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

void Control::requestObject(Object* theTarget) {
  switch (theTarget->type()) {
    case kObjectAudio:
      audioManager.requestAudio((Audio*)theTarget);
      break;
    case kObjectVideo:
      videoManager.requestVideo((Video*)theTarget);
      break;
  }
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
  
  _updateView(StateNode, true);
  
  videoManager.flush();
  
  if (firstSwitch) {
    firstSwitch = false;
  }
  else {
    renderManager.blendNextUpdate(true);
  }
  
  cameraManager.setViewport(config.displayWidth, config.displayHeight);
  
  if (theTarget) {
    switch (theTarget->type()) {
      case kObjectRoom:
        audioManager.clear();
        feedManager.clear(); // Clear all pending feeds
        
        if (currentNode()) {
          if (currentNode()->isSlide())
            cameraManager.unlock();
        }
        
        _currentRoom = (Room*)theTarget;
        _scene->setRoom((Room*)theTarget);
        timerManager.setLuaObject(_currentRoom->luaObject());
        
        if (!_currentRoom->hasNodes()) {
          log.warning(kModControl, "%s: %s", kString12005, _currentRoom->name().c_str());
          return;
        }
        
        textureManager.setRoomToPreload(_currentRoom);
        
        if (_eventHandlers.hasEnterRoom)
          script.processCallback(_eventHandlers.enterRoom, 0);
        
        if (_currentRoom->hasEnterEvent())
          script.processCallback(_currentRoom->enterEvent(), 0);
        
        break;
      case kObjectSlide:
        if (_currentRoom) {
          Node* node = (Node*)theTarget;
          node->setPreviousNode(this->currentNode());
          
          if (!_currentRoom->switchTo(node)) {
            log.error(kModControl, "%s: %s (%s)", kString12010, node->name().c_str(), _currentRoom->name().c_str()); // Bad slide
            return;
          }
          
          if (_directControlActive)
            _directControlActive = false;
          
          cameraManager.lock();
        }
        else {
          log.error(kModControl, "%s", kString12009);
          return;
        }
        
        break;
      case kObjectNode:
        audioManager.clear();
        if (currentNode()->isSlide())
          cameraManager.unlock();
        
        Node* node = (Node*)theTarget;
        Room* room = node->parentRoom();
        
        if (room) {
          if (room != _currentRoom) {
            feedManager.clear(); // Clear all pending feeds
            _currentRoom = room;
            _scene->setRoom(room);
            timerManager.setLuaObject(_currentRoom->luaObject());
            textureManager.setRoomToPreload(_currentRoom);
            
            if (_eventHandlers.hasEnterRoom)
              script.processCallback(_eventHandlers.enterRoom, 0);
            
            if (_currentRoom->hasEnterEvent())
              script.processCallback(_currentRoom->enterEvent(), 0);
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
        
        if (_eventHandlers.hasEnterNode)
          script.processCallback(_eventHandlers.enterNode, 0);
        
        break;
    }
  }
  else {
    // Only slides switch to NULL targets, so we check whether the new object is another slide.
    // If it isn't, we unlock the camera.
    if (_currentRoom) {
      Node* current = this->currentNode();
      Node* previous = current->previousNode();
      
      _currentRoom->switchTo(previous);
      
      if (current->slideReturn()) {
        script.processCallback(current->slideReturn(), current->luaObject());
      }
      
      if (!previous->isSlide()) {
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
      textureManager.flush();
      
      if (current->hasSpots()) {
        current->beginIteratingSpots();
        do {
          Spot* spot = current->currentSpot();
          
          if (spot->hasAudio()) {
            Audio* audio = spot->audio();
            
            // Request the audio
            audioManager.requestAudio(audio);
            audio->setPosition(spot->face(), spot->origin());
            audio->setDefaultFadeLevel(spot->volume());
          }
          
          // TODO: Merge the video autoplay with spot properties
          // TODO: Decide after video finishes playing if last frame is showed or removed
          if (spot->hasVideo()) {
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
            textureManager.requestTexture(spot->texture());
            
            // Only resize if nothing but origin
            if (spot->vertexCount() == 1)
              spot->resize(spot->texture()->width(), spot->texture()->height());
          }
          
          if (spot->hasFlag(kSpotAuto))
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
      system.setTitle(title);
    }
    
    // This has to be done every time so that room audios keep playing
    if (_currentRoom->hasAudios() && (!_currentRoom->currentNode()->isSlide() && theTarget != NULL)) {
      std::vector<Audio*>::iterator it;
      std::vector<Audio*> arrayOfAudios = _currentRoom->arrayOfAudios();
      
      it = arrayOfAudios.begin();
      
      while (it != arrayOfAudios.end()) {
        if ((*it)->state() != kAudioPlaying)
          (*it)->fadeIn();
        
        audioManager.requestAudio((*it));
        (*it)->play();
        
        ++it;
      }
    }
  }
  
  audioManager.flush();
  
  cameraManager.stopPanning();
}
  
void Control::walkTo(Object* theTarget) {
  this->switchTo(theTarget);
  
  cameraManager.simulateWalk();
  Node* current = this->currentNode();
  
  // Finally, check if must play a single footstep
  if (current->hasFootstep()) {
    current->footstep()->unload();
    audioManager.requestAudio(current->footstep());
    current->footstep()->setFadeLevel(1.0f); // FIXME: Shouldn't be necessary to do this
    current->footstep()->play();
  }
  else {
    if (_currentRoom->hasDefaultFootstep()) {
      _currentRoom->defaultFootstep()->unload();
      audioManager.requestAudio(_currentRoom->defaultFootstep());
      _currentRoom->defaultFootstep()->setFadeLevel(1.0f); // FIXME: Shouldn't be necessary to do this
      _currentRoom->defaultFootstep()->play();
    }
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

void Control::terminate() {
  _isRunning = false;
  
  audioManager.terminate();
  timerManager.terminate();
  videoManager.terminate();
  
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

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void Control::_processAction(){
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
      cursorManager.removeAction();
      walkTo(action->target);
      break;
  }
}

void Control::_updateView(int state, bool inBackground) {
  // TODO: Suspend all operations when doing a switch
  // FIXME: Add a render stack of Objects, especially for overlays
  // IMPORTANT: Ensure this function is thread-safe when switching rooms or nodes
  
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
      _scene->drawSpots();
      
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
