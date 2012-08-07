////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGAudioManager.h"
#include "DGCameraManager.h"
#include "DGConfig.h"
#include "DGConsole.h"
#include "DGControl.h"
#include "DGCursorManager.h"
#include "DGFeedManager.h"
#include "DGFontManager.h"
#include "DGInterface.h"
#include "DGLog.h"
#include "DGNode.h"
#include "DGRenderManager.h"
#include "DGRoom.h"
#include "DGScene.h"
#include "DGScript.h"
#include "DGSpot.h"
#include "DGState.h"
#include "DGSystem.h"
#include "DGTextureManager.h"
#include "DGTimerManager.h"
#include "DGVideoManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGControl::DGControl() {
    audioManager = &DGAudioManager::getInstance();
    cameraManager = &DGCameraManager::getInstance();    
    config = &DGConfig::getInstance();  
    cursorManager = &DGCursorManager::getInstance();  
    feedManager = &DGFeedManager::getInstance();
    fontManager = &DGFontManager::getInstance();
    log = &DGLog::getInstance();
    renderManager = &DGRenderManager::getInstance();    
    script = &DGScript::getInstance();
    system = &DGSystem::getInstance();
    timerManager = &DGTimerManager::getInstance();
    videoManager = &DGVideoManager::getInstance();
    
    _currentRoom = NULL;
    
    _fpsCount = 0;
    _sleepTimer = 0;
    
    _isInitialized = false;
    _isShuttingDown = false;
	_isRunning = false;
    
    // This is used to randomize the color of certain spots,
    // should be called once
    srand((unsigned)time(0));
    
    // For precaution
    
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
    
    for (int i = 0; i <= DGMaxHotKeys; i++)
		_hotkeyData[i].enabled = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGControl::~DGControl() {
    if (_isInitialized) {
        delete _console;
        delete _interface;
        delete _state;
        delete _scene;
        delete _textureManager;
    }
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGControl::init() {
    log->info(DGModControl, "%s: %d.%d.%d%s%d", DGMsg030000, DGVersionMajor,
              DGVersionMinor, DGVersionRelease, DGVersionType, DGVersionBuild);
    
	log->trace(DGModControl, "%s", DGMsg030001);
    
    renderManager->init();
    renderManager->resetView(); // Test for errors

    cameraManager->init();
    cameraManager->setViewport(config->displayWidth, config->displayHeight);
    
    // Init the audio manager
    audioManager->init();
    
    // Init the font library
    fontManager->init();
    
    // Init the video manager
    videoManager->init();
    
    feedManager->init();
    
    _interface = new DGInterface;
    _scene = new DGScene;
    
    _state = new DGState;
    _textureManager = new DGTextureManager;
    
    _dragTimer = timerManager->createManual(DGTimeToStartDragging);
    timerManager->disable(_dragTimer);
    
    _console = new DGConsole;
    if (config->debugMode) {
        // Console must be initialized after the Font Manager
        _console->init();
        _console->enable();
    }
    
    // If the splash screen is enabled, load its data and set the correct state
    if (config->showSplash) {
        _cancelSplash = false;
        _scene->loadSplash();
        _state->set(DGStateSplash);
        cursorManager->disable();
        renderManager->fadeInNextUpdate();
    }
    else renderManager->resetFade();
    
    _directControlActive = true;
    _isInitialized = true;
	_isRunning = true;
}

DGNode* DGControl::currentNode() {
    if (_currentRoom)
        return _currentRoom->currentNode();
    else
        return NULL;
}

DGRoom* DGControl::currentRoom() {
    return _currentRoom;
}

void DGControl::cutscene(const char* fileName) {
    _scene->loadCutscene(fileName);
    _state->set(DGStateCutscene);
    cursorManager->fadeOut();
}

bool DGControl::isDirectControlActive() {
	if (config->controlMode == DGMouseFixed)
		return _directControlActive;
	else
		return false;
}
    
void DGControl::lookAt(float horizontal, float vertical, bool instant) {
    if (instant) {
        cameraManager->setAngleHorizontal(horizontal);
        cameraManager->setAngleVertical(vertical);
    }
    else {
        cameraManager->setTargetAngle(horizontal, vertical);
        _state->set(DGStateLookAt);
        cursorManager->fadeOut();
    }
}

void DGControl::processFunctionKey(int aKey) {
    int idx = 0;
    
    switch (aKey) {
		case DGKeyF1: idx = 1; break;
		case DGKeyF2: idx = 2; break;
		case DGKeyF3: idx = 3; break;
		case DGKeyF4: idx = 4; break;
		case DGKeyF5: idx = 5; break;
		case DGKeyF6: idx = 6; break;
		case DGKeyF7: idx = 7; break;
		case DGKeyF8: idx = 8; break;
		case DGKeyF9: idx = 9; break;
		case DGKeyF10: idx = 10; break;
		case DGKeyF11: idx = 11; break;
		case DGKeyF12: idx = 12; break;         
	}
	
    if (idx) {
        if (_hotkeyData[idx].enabled)
           script->processCommand(_hotkeyData[idx].line);
    }
}

void DGControl::processKey(int aKey, int eventFlags) {
    switch (eventFlags) {
        case DGKeyEventDown:
            switch (aKey) {
                case DGKeyEsc:
                    if (_state->current() == DGStateSplash) {
                        _cancelSplash = true;
                    }
                    else {
                        if (feedManager->isPlaying())
                            feedManager->cancel();
                        else {
                            if (!_isShuttingDown) {
                                _scene->fadeOut();
                                _shutdownTimer = timerManager->createManual(1);
                                _isShuttingDown = true;
                            }
                        }
                    }
                    break;
                case DGKeyQuote:
                case DGKeyTab:
                    _console->toggle();
                    break;
                case DGKeySpacebar:
                    if (_console->isHidden())
                        config->showHelpers = !config->showHelpers;
                    break;
                case 'w':
                case 'W':
                    if (_console->isHidden()) {
                        cameraManager->pan(DGCurrent, 0);
                    }
                    break;
                case 'a':
                case 'A':
                    if (_console->isHidden()) {
                        cameraManager->pan(0, DGCurrent);
                    }
                    break;
                case 's':
                case 'S':
                    if (_console->isHidden()) {
                        cameraManager->pan(DGCurrent, config->displayHeight);
                    }
                    break;
                case 'd':
                case 'D':
                    if (_console->isHidden()) {
                        cameraManager->pan(config->displayWidth, DGCurrent);
                    }
                    break;
            }
            
            // Process these keys only when the console is visible
            if (!_console->isHidden()) {
                switch (aKey) {
                    case DGKeyEsc:
                    case DGKeyQuote:
                    case DGKeyTab:
                        // Ignore these
                        break;
                    case DGKeyBackspace:
                        _console->deleteChar();
                        break;
                    case DGKeyEnter:
                        _console->execute();
                        break;
                    default:
                        _console->inputChar(aKey);
                        break;            
                }  
            }
            break;
            
        case DGKeyEventModified:
            switch (aKey) {
                case 'f':
                case 'F':
                    config->fullScreen = !config->fullScreen;
                    system->toggleFullScreen();
                    break;
            }
            break;
            
        case DGKeyEventUp:
            if (_console->isHidden()) {
                switch (aKey) {
                    case 'w':
                    case 'W':
                        cameraManager->pan(DGCurrent, config->displayHeight / 2);
                        break;
                    case 'a':
                    case 'A':
                        cameraManager->pan(config->displayWidth / 2, DGCurrent);
                        break;
                    case 's':
                    case 'S':
                        cameraManager->pan(DGCurrent, config->displayHeight / 2);
                        break;
                    case 'd':
                    case 'D':
                        cameraManager->pan(config->displayWidth / 2, DGCurrent);
                        break;
                }
            }
            break;
    }
}

void DGControl::processMouse(int x, int y, int eventFlags) {
    // TODO: Horrible nesting of IFs here... improve
    
    if (config->controlMode == DGMouseFixed) {
        if (!_directControlActive) {
            cursorManager->updateCoords(x, y);
        }
    }
    else cursorManager->updateCoords(x, y);
        
    if (!cursorManager->isEnabled() || cursorManager->isFading()) {
        // Ignore all the rest
        cursorManager->setCursor(DGCursorNormal);
        cameraManager->stopPanning();
        return;
    }
            
    if ((eventFlags == DGMouseEventMove) && (_eventHandlers.hasMouseMove))
        script->processCallback(_eventHandlers.mouseMove, 0);
    
    if ((eventFlags == DGMouseEventUp) && _eventHandlers.hasMouseButton) {
        script->processCallback(_eventHandlers.mouseButton, 0);
    }
    
    if (config->controlMode == DGMouseFixed) {
        if (eventFlags == DGMouseEventRightUp) {
            _directControlActive = !_directControlActive;
        }
    }
    else {
        if ((eventFlags == DGMouseEventRightUp) && _eventHandlers.hasMouseRightButton) {
            script->processCallback(_eventHandlers.mouseRightButton, 0);
        }
    }
    
    // The overlay system is handled differently than the spots, so we
    // do everything here. Eventually, we should tide up things a little.

    bool canProcess = false;
    
    if (config->controlMode == DGMouseFixed) {
        if (!_directControlActive)
            canProcess = true;
    }
    else {
        if (!cursorManager->isDragging())
            canProcess = true;
    }
    
    // TODO: Use active overlays only
    if (canProcess) {
        if (_interface->scanOverlays()) {
            if ((eventFlags == DGMouseEventUp) && cursorManager->hasAction()) {
                _processAction();
                
                // Repeat the scan in case the button is no longer visible  
                _interface->scanOverlays();
                
                // Stop processing spots
                return;
            }
        }
    }

    switch (config->controlMode) {
        case DGMouseFree:
            if (eventFlags == DGMouseEventMove) {
                if (!cursorManager->onButton()) {
                    cameraManager->pan(x, y);
                    
                    // When in free mode, we first check if camera is panning because
                    // we don't want the "not dragging" cursor
                    if (cameraManager->isPanning() && !cursorManager->hasAction()) {
                        cursorManager->setCursor(cameraManager->cursorWhenPanning());
                    }
                    else cursorManager->setCursor(DGCursorNormal);

                }
                else cameraManager->stopPanning();
            }
            
            if (eventFlags == DGMouseEventUp) {
                if (cursorManager->hasAction())
                    _processAction();
            }
            break;
            
        case DGMouseFixed:
            if (_directControlActive) {
                cursorManager->updateCoords(config->displayWidth / 2, config->displayHeight / 2); // Forced
            
                if (eventFlags == DGMouseEventMove) {
                    cameraManager->directPan(x, y);
                }
            }
            
            if (eventFlags == DGMouseEventUp) {
                if (cursorManager->hasAction()) {
                    _processAction();
                }
            }
            break;
            
        case DGMouseDrag:
            if (eventFlags == DGMouseEventDrag) {
                if (cursorManager->isDragging()) {
                    cameraManager->pan(x, y);
                    cursorManager->setCursor(cameraManager->cursorWhenPanning());
                }
            }
            
            // FIXME: Start dragging a few milliseconds after the mouse if down
            if (eventFlags == DGMouseEventDown && !cursorManager->onButton()) {
                timerManager->enable(_dragTimer);
            }
            
            if (eventFlags == DGMouseEventUp) {
                timerManager->disable(_dragTimer); // Cancel the current check
                
                if (cursorManager->isDragging()) {
                    cursorManager->setDragging(false);
                    cameraManager->stopDragging();
                }
                else if (cursorManager->hasAction()) {
                    _processAction();
                }
            }
            
            break;
    }
}

void DGControl::registerGlobalHandler(int forEvent, int handlerForLua) {
    switch (forEvent) {
		case DGEventEnterNode:
			_eventHandlers.enterNode = handlerForLua;
			_eventHandlers.hasEnterNode = true;
			break;
		case DGEventLeaveNode:
			_eventHandlers.leaveNode = handlerForLua;
			_eventHandlers.hasLeaveNode = true;
			break;
		case DGEventEnterRoom:
			_eventHandlers.enterRoom = handlerForLua;
			_eventHandlers.hasEnterRoom = true;
			break;
		case DGEventLeaveRoom:
			_eventHandlers.leaveRoom = handlerForLua;
			_eventHandlers.hasLeaveRoom = true;
			break;
		case DGEventPreRender:
			_eventHandlers.preRender = handlerForLua;
			_eventHandlers.hasPreRender = true;
			break;
		case DGEventPostRender:
			_eventHandlers.postRender = handlerForLua;
			_eventHandlers.hasPostRender = true;
			break;
		case DGEventMouseButton:
			_eventHandlers.mouseButton = handlerForLua;
			_eventHandlers.hasMouseButton = true;
			break;
		case DGEventMouseRightButton:
			_eventHandlers.mouseRightButton = handlerForLua;
			_eventHandlers.hasMouseRightButton = true;
			break;
		case DGEventMouseMove:
			_eventHandlers.mouseMove = handlerForLua;
			_eventHandlers.hasMouseMove = true;
			break;	
		case DGEventResize:
			_eventHandlers.resize = handlerForLua;
			_eventHandlers.hasResize = true;
			break;	            
	}
}

void DGControl::registerHotkey(int aKey, const char* luaCommandToExecute) {
    int idx = 0;
    
    switch (aKey) {
		case DGKeyF1: idx = 1; break;
		case DGKeyF2: idx = 2; break;
		case DGKeyF3: idx = 3; break;
		case DGKeyF4: idx = 4; break;
		case DGKeyF5: idx = 5; break;
		case DGKeyF6: idx = 6; break;
		case DGKeyF7: idx = 7; break;
		case DGKeyF8: idx = 8; break;
		case DGKeyF9: idx = 9; break;
		case DGKeyF10: idx = 10; break;
		case DGKeyF11: idx = 11; break;
		case DGKeyF12: idx = 12; break;
    }
    
    if (idx) {
        _hotkeyData[idx].enabled = true;
        _hotkeyData[idx].value = aKey;
        strncpy(_hotkeyData[idx].line, luaCommandToExecute, DGMaxLogLength);
    }
}

void DGControl::registerObject(DGObject* theTarget) {
    switch (theTarget->type()) {
        case DGObjectNode:
             _textureManager->requestBundle((DGNode*)theTarget);
            break;
        case DGObjectOverlay:
            _interface->addOverlay((DGOverlay*)theTarget);
            break;
        case DGObjectRoom: 
            _arrayOfRooms.push_back((DGRoom*)theTarget);
            break;
    }
}

void DGControl::reshape(int width, int height) {
    int size = (width * DGDefCursorSize) / 1920;
    
    if (size > DGMaxCursorSize)
        size = DGMaxCursorSize;
    else if (size < DGMinCursorSize)
        size = DGMinCursorSize;
    
    config->displayWidth = width;
    config->displayHeight = height;
    
    cameraManager->setViewport(width, height);
    cursorManager->setSize(size);
    feedManager->reshape();
    renderManager->reshape();
    
    if (_eventHandlers.hasResize)
        script->processCallback(_eventHandlers.resize, 0);    
}

void DGControl::sleep(int forSeconds) {
    _sleepTimer = timerManager->createManual(forSeconds);
    _state->set(DGStateSleep);
    cursorManager->fadeOut();
}

void DGControl::switchTo(DGObject* theTarget) {
    static bool firstSwitch = true;
    bool performWalk;
    
    _updateView(DGStateNode, true);
    
    system->suspendThread(DGVideoThread);
    videoManager->flush();
    
    if (theTarget) {
        switch (theTarget->type()) {
            case DGObjectRoom:
                audioManager->clear();
                feedManager->clear(); // Clear all pending feeds
                
                renderManager->blendNextUpdate(true);
                
                _currentRoom = (DGRoom*)theTarget;
                _scene->setRoom((DGRoom*)theTarget);
                timerManager->setLuaObject(_currentRoom->luaObject());
                
                if (!_currentRoom->hasNodes()) {
                    log->warning(DGModControl, "%s: %s", DGMsg130000, _currentRoom->name());
                    return;
                }
                performWalk = true;
                
                break;
            case DGObjectSlide:
                renderManager->blendNextUpdate();
                
                if (_currentRoom) {
                    DGNode* node = (DGNode*)theTarget;
                    node->setPreviousNode(this->currentNode());
                    
                    if (!_currentRoom->switchTo(node)) {
                        log->error(DGModControl, "%s: %s (%s)", DGMsg230002, node->name(), _currentRoom->name()); // Bad slide
                        return;
                    }
                    
                    if (_directControlActive)
                        _directControlActive = false;
                    
                    cameraManager->lock();
                    performWalk = false;
                }
                else {
                    log->error(DGModControl, "%s", DGMsg230001);
                    return;
                }

                break;
            case DGObjectNode:
                audioManager->clear();
                
                renderManager->blendNextUpdate(true);
                
                if (_currentRoom) {
                    DGNode* node = (DGNode*)theTarget;
                    if (!_currentRoom->switchTo(node)) {
                        log->error(DGModControl, "%s: %s (%s)", DGMsg230000, node->name(), _currentRoom->name()); // Bad node
                        return;
                    }
                    performWalk = true;
                }
                else {
                    log->error(DGModControl, "%s", DGMsg230001);
                    return;
                }
                
                break;
        }
    }
    else {
        // Only slides switch to NULL targets, so we check whether the new object is another slide.
        // If it isn't, we unlock the camera.
        if (_currentRoom) {
            renderManager->blendNextUpdate();
            
            DGNode* currentNode = this->currentNode();
            DGNode* previousNode = currentNode->previousNode();
            
            _currentRoom->switchTo(previousNode);
            
            if (currentNode->slideReturn()) {
                script->processCallback(currentNode->slideReturn(), currentNode->luaObject());
            }
            
            if (!previousNode->isSlide())
                cameraManager->unlock();
            
            performWalk = false;
        }
    }
    
    if (_currentRoom) {
        if (_currentRoom->hasNodes()) {
            // Now we proceed to load the textures of the current node
            DGNode* currentNode = _currentRoom->currentNode();
            _textureManager->flush();
                
            if (currentNode->hasSpots()) {                
                currentNode->beginIteratingSpots();
                do {
                    DGSpot* spot = currentNode->currentSpot();
                    
                    if (spot->hasAudio()) {
                        DGAudio* audio = spot->audio();
                        
                        // Request the audio
                        audioManager->requestAudio(audio);
                        audio->setPosition(spot->face(), spot->origin());
                        audio->setDefaultFadeLevel(spot->volume());
                    }
                    
                    // TODO: Merge the video autoplay with spot properties
                    // TODO: Decide after video finishes playing if texture is showed or removed
                    if (spot->hasVideo()) {
                        DGVideo* video = spot->video();
                        videoManager->requestVideo(video);
                        
                        if (video->isLoaded()) {
                            if (!spot->hasTexture()) {
                                DGTexture* texture = new DGTexture;
                                spot->setTexture(texture);
                            }
                            
                            video->play();
                            
                            DGFrame* frame = video->currentFrame();
                            spot->texture()->loadRawData(frame->data, frame->width, frame->height);
                            
                            video->pause();
                        }
                    }
                    
                    if (spot->hasTexture()) {
                        _textureManager->requestTexture(spot->texture());
                        
                        // Only resize if nothing but origin
                        if (spot->vertexCount() == 1)
                            spot->resize(spot->texture()->width(), spot->texture()->height());
                    }
                    
                    if (spot->hasFlag(DGSpotAuto))
                        spot->play();
                } while (currentNode->iterateSpots());
            }
            else {
                log->warning(DGModControl, "%s", DGMsg130001);
            }
            
            // Prepare the name for the window
            char title[DGMaxObjectName];
            snprintf(title, DGMaxObjectName, "%s (%s, %s)", config->script(), 
                     _currentRoom->name(), currentNode->description());
            system->setTitle(title);
        }
        
        // This has to be done every time so that room audios keep playing
        if (_currentRoom->hasAudios() && (!_currentRoom->currentNode()->isSlide() && theTarget != NULL)) {
            vector<DGAudio*>::iterator it;
            vector<DGAudio*> arrayOfAudios = _currentRoom->arrayOfAudios();
            
            it = arrayOfAudios.begin();
            
            while (it != arrayOfAudios.end()) {
                if ((*it)->state() != DGAudioPlaying)
                    (*it)->fadeIn();
                
                audioManager->requestAudio((*it));                
                (*it)->play();
                
                it++;
            }
        }
        
        if (!firstSwitch && performWalk) {
            cameraManager->simulateWalk();
            
            // Finally, check if must play a single footstep
            if (_currentRoom->hasDefaultFootstep()) {
                _currentRoom->defaultFootstep()->unload();
                audioManager->requestAudio(_currentRoom->defaultFootstep());
                _currentRoom->defaultFootstep()->setFadeLevel(1.0f); // FIXME: Shouldn't be necessary to do this
                _currentRoom->defaultFootstep()->play();
            }
        }
    }
    
    audioManager->flush();
    
    system->resumeThread(DGVideoThread);
    
    cameraManager->stopPanning();
    
    if (firstSwitch)
        firstSwitch = false;
}

void DGControl::syncSpot(DGSpot* spot) {
    _syncedSpot = spot;
    _state->set(DGStateVideoSync);
    cursorManager->fadeOut(); 
}

void DGControl::takeSnapshot() {
    bool previousTexCompression = config->texCompression;
    config->texCompression = false;
    
    time_t rawtime;
	struct tm* timeinfo;
	char buffer[DGMaxFileLength];
    DGTexture texture(0, 0, 0);
    
	time(&rawtime);
	timeinfo = localtime (&rawtime);
    
	strftime(buffer, DGMaxFileLength, "snap-%Y-%m-%d-%Hh%Mm%Ss", timeinfo);
    
    system->update();
    cameraManager->beginOrthoView();
    renderManager->drawPostprocessedView();
    texture.bind();
    renderManager->copyView();
    cameraManager->endOrthoView();
    
    texture.saveToFile(buffer);   
    
    config->texCompression = previousTexCompression;
}

bool DGControl::profiler() {
	if (_isRunning) {
		// Store the last FPS count and reset
		config->setFramesPerSecond(_fpsCount);
		_fpsCount = 0;

		return true;
	}

	return false;
}

void DGControl::terminate() {
	_isRunning = false;
    system->terminate();
}

bool DGControl::update() {
	if (_isRunning) {
		switch (_state->current()) {
			case DGStateLookAt:
				cameraManager->panToTargetAngle();
				_updateView(_state->previous(), false);
            
				if (!cameraManager->isPanning()) {
					 _state->setPrevious();
					cursorManager->fadeIn();
					script->resume(); 
				}
				break;
			case DGStateSleep:
				if (timerManager->checkManual(_sleepTimer)) {
					_state->setPrevious();
					cursorManager->fadeIn();
					script->resume();
				}
				else {
					_updateView(_state->previous(), false);
				}

				break;
			case DGStateVideoSync:
				_updateView(_state->previous(), false);
            
				if (!_syncedSpot->video()->isPlaying()) {
					_state->setPrevious();
					cursorManager->fadeIn();
					script->resume(); 
				}
				break;      
			default:
				_updateView(_state->current(), false);
				break;
		}
        
        if (config->controlMode == DGMouseDrag) {
            if (timerManager->checkManual(_dragTimer)) {
                DGPoint position = cursorManager->position();
                cameraManager->startDragging(position.x, position.y);
                cursorManager->setDragging(true);
                
                timerManager->disable(_dragTimer);            
            }
        }
        
		if (_isShuttingDown) {
			if (timerManager->checkManual(_shutdownTimer)) {
				this->terminate();
			}
		}

		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void DGControl::_processAction(){
    DGAction* action = cursorManager->action();
    
    switch (action->type) {
        case DGActionFunction:
            script->processCallback(action->luaHandler, action->luaObject);
            break;
        case DGActionFeed:
            if (action->hasFeedAudio) {
                feedManager->showAndPlay(action->feed, action->feedAudio);             
            }
            else feedManager->show(action->feed);
            break;
        case DGActionSwitch:
            cursorManager->removeAction();
            switchTo(action->target);
            break;
    }
}

void DGControl::_updateView(int state, bool inBackground) {
    // TODO: Suspend all operations when doing a switch
    // FIXME: Add a render stack of DGObjects, especially for overlays
    // IMPORTANT: Ensure this function is thread-safe when
    // switching rooms or nodes
    
    // Setup the scene
    
    _scene->clear();
    
    // Do this in a viewtimer
    switch (state) {
        case DGStateCutscene:
            if (!_scene->drawCutscene()) {
                _scene->unloadCutscene();
                _state->setPrevious();
                cursorManager->fadeIn();
                script->resume();
            }
            break;
        case DGStateNode:
            _scene->scanSpots();
            _scene->drawSpots();
            
            if (!inBackground) {
                _interface->drawHelpers();
                _interface->drawOverlays();
                feedManager->update();
                _interface->drawCursor();
            }
            
            break;
        case DGStateSplash:
            static int handlerIn = timerManager->createManual(3);
            static int handlerOut = timerManager->createManual(4);
            
            _scene->drawSplash();
            
            if (timerManager->checkManual(handlerIn)) {
                _scene->fadeOut();
            }
            
            if (timerManager->checkManual(handlerOut) || _cancelSplash) {
                cursorManager->enable();
                renderManager->clearView();
                renderManager->resetFade();
                _state->set(DGStateNode);
                _scene->unloadSplash();
                
                script->execute();
            }
            
            break;
    }
    
    if (!inBackground) {
        // User post render operations, supporting textures
        if (_eventHandlers.hasPostRender)
            script->processCallback(_eventHandlers.postRender, 0);
    }
    
    // General fade, affects every graphic on screen
    renderManager->fadeView();
    
    // Debug info, if enabled
    if (_console->isEnabled() && !inBackground) {
        _fpsCount++;
        // BUG: This causes a crash sometimes. Why?
        _console->update();
        
        // We do this here in case the command changes the viewport
        cameraManager->endOrthoView();
        
        if (_console->isReadyToProcess()) {
            char command[DGMaxLogLength];
            _console->getCommand(command);
            script->processCommand(command);
        }
    }
    else cameraManager->endOrthoView();
    
    audioManager->setOrientation(cameraManager->orientation());
    
    system->suspendThread(DGTimerThread);
    timerManager->process();
    system->resumeThread(DGTimerThread);
    
    if (!inBackground) {
        // Flush the buffers
        system->update();
    }
}
