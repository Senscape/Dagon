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
#include "DGButton.h"
#include "DGCamera.h"
#include "DGConfig.h"
#include "DGConsole.h"
#include "DGControl.h"
#include "DGCursorManager.h"
#include "DGFeedManager.h"
#include "DGFontManager.h"
#include "DGImage.h"
#include "DGLog.h"
#include "DGNode.h"
#include "DGOverlay.h"
#include "DGRender.h"
#include "DGRoom.h"
#include "DGScript.h"
#include "DGSpot.h"
#include "DGState.h"
#include "DGSystem.h"
#include "DGTextureManager.h"
#include "DGTimerManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGControl::DGControl() {
    audioManager = &DGAudioManager::getInstance();
    config = &DGConfig::getInstance();  
    cursorManager = &DGCursorManager::getInstance();  
    feedManager = &DGFeedManager::getInstance();
    fontManager = &DGFontManager::getInstance();
    log = &DGLog::getInstance();
    script = &DGScript::getInstance();
    system = &DGSystem::getInstance();
    timerManager = &DGTimerManager::getInstance();
    
    _currentRoom = NULL;
    
    _fpsCount = 0;
    _fpsLastCount = 0;
    
    _isInitialized = false;
    
    // This is used to randomize the color of certain spots,
    // should be called once
    srand((unsigned)time(0)); 
    
    for (int i = 0; i <= DGMaxHotKeys; i++)
		_hotKeyData[i].enabled = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGControl::~DGControl() {
    if (_isInitialized) {
        delete _camera;
        delete _console;
        delete _render;
        delete _state;
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
    
    _render = new DGRender;
    _render->init();
    _render->resetScene(); // Test for errors
    
    _camera = new DGCamera;
    _camera->setViewport(config->displayWidth, config->displayHeight);
    
    // Init the audio manager
    audioManager->init();
    
    // Load the default font with a small height
    fontManager->init();
    
    _console = new DGConsole;
    _console->init(); // Must be initialized after the Font Manager
    _consoleFont = fontManager->loadDefault();
    
    feedManager->init();
    
    _state = new DGState;
    _textureManager = new DGTextureManager;
    
    _canDrawSpots = false;
    _isInitialized = true;
    
    // If the splash screen is enabled, load its data and set the correct state
    if (config->showSplash) {
        _render->loadSplash();
        _state->set(DGStateSplash);
        _render->fadeInNextUpdate();
    }
    else _render->resetFade();
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
        //if (_hotKeyData[idx].enabled)
        //DGScriptDoLine(hotkey[idx].line);
        log->trace(DGModControl, "Index = %d", idx); // Temp
    }
}

void DGControl::processKey(int aKey, bool isModified) {
    switch (aKey) {
		case DGKeyEsc:
            _render->fadeOutNextUpdate();
            timerManager->createInternal(1, terminate);
			break;
		case DGKeyQuote:
		case DGKeyTab:         
			_console->toggle();
			break;
		case DGKeyBackspace:
			_console->deleteChar();
			break;
		case DGKeyEnter:
			_console->execute();
			break;
		case DGKeySpacebar:
            if (!_console->isEnabled())
                config->showHelpers = !config->showHelpers;
            else _console->inputChar(aKey);
			break;            
        case 'f':
        case 'F':
            if (isModified) {
                system->toggleFullScreen();
            }
            else _console->inputChar(aKey);
            break;
		default:
            _console->inputChar(aKey);
			break;            
	}
}

void DGControl::processMouse(int x, int y, int eventFlags) {
    if ((eventFlags & DGMouseEventMove) && (_eventHandlers.hasMouseMove))
        script->processCallback(_eventHandlers.mouseMove, 0);
    
    if ((eventFlags & DGMouseEventUp) && _eventHandlers.hasMouseButton) {
        script->processCallback(_eventHandlers.mouseButton, 0);
    }
    
    // The overlay system is handled differently than the spots, so we
    // do everything here. Eventually, we should tide up things a little.

    // TODO: Use active overlays only
    if (!cursorManager->isDragging()) {
        if (_scanOverlays()) {
            if ((eventFlags & DGMouseEventUp) && cursorManager->hasAction()) {
                _processAction();
                
                // Repeat the scan in case the button is no longer visible  
                _scanOverlays();
                
                // Stop processing spots
                return;
            }
        }
    }

    switch (config->controlMode) {
        case DGMouseFree:
            if (eventFlags & DGMouseEventMove) {
                if (!cursorManager->onButton()) {
                    _camera->pan(x, y);
                    
                    // When in free mode, we first check if camera is panning because
                    // we don't want the "not dragging" cursor
                    if (_camera->isPanning()) { 
                        cursorManager->setCursor(_camera->cursorWhenPanning());
                    }
                }
                else _camera->stopPanning();
            }
            
            if (eventFlags & DGMouseEventUp) {
                if (cursorManager->hasAction())
                    _processAction();
            }
            break;
            
        case DGMouseDrag:
            if (eventFlags & DGMouseEventDrag) {
                if (cursorManager->isDragging()) {
                    _camera->pan(x, y);
                    cursorManager->setCursor(_camera->cursorWhenPanning());
                }
                else cursorManager->setDragging(true);
            }
            
            // FIXME: Start dragging a few milliseconds after the mouse if down
            if (eventFlags & DGMouseEventDown) {
                _camera->startDragging(x, y);
            }
            
            if (eventFlags & DGMouseEventUp) {
                if (cursorManager->isDragging()) {
                    cursorManager->setDragging(false);
                    _camera->stopDragging();
                }
                else if (cursorManager->hasAction()) {                    
                    _processAction();
                }
            }
            
            break;
    }
    
    cursorManager->updateCoords(x, y);
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

void DGControl::registerHotKey(int aKey, const char* luaCommandToExecute) {
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
        _hotKeyData[idx].enabled = true;
        _hotKeyData[idx].value = aKey;
        strncpy(_hotKeyData[idx].line, luaCommandToExecute, DGMaxLogLength);
    }
}

void DGControl::registerObject(DGObject* theTarget) {
    switch (theTarget->type()) {
        case DGObjectNode:
             _textureManager->requestBundle((DGNode*)theTarget);
            break;
        case DGObjectOverlay:
            _arrayOfOverlays.push_back((DGOverlay*)theTarget);
            break;
        case DGObjectRoom: 
            _arrayOfRooms.push_back((DGRoom*)theTarget);
            break;
    }
}

void DGControl::reshape(int width, int height) {
    config->displayWidth = width;
    config->displayHeight = height;
    
    _camera->setViewport(width, height);
    
    if (_eventHandlers.hasResize)
        script->processCallback(_eventHandlers.resize, 0);    
}

void DGControl::sleep(int forMilliseconds) {
    // Must implement
}

void DGControl::switchTo(DGObject* theTarget) {
    static bool firstSwitch = true;
    
    if (!firstSwitch) {
        // FIXME: This code should be integrated with the general update()
        _render->clearScene();
        _camera->update();
        _drawSpots();
        _render->blendNextUpdate();
        _camera->simulateWalk();
        ///////////////////////////
    }
    else firstSwitch = false;
    
    audioManager->clear();
    
    switch (theTarget->type()) {
        case DGObjectRoom:
            _currentRoom = (DGRoom*)theTarget;
            
            if (!_currentRoom->hasNodes())
                log->warning(DGModControl, "%s: %s", DGMsg130000, _currentRoom->name());

            break;
        case DGObjectNode:
            if (_currentRoom) {
                DGNode* node = (DGNode*)theTarget;
                if (!_currentRoom->switchTo(node))
                    log->error(DGModControl, "%s: %s (%s)", DGMsg230000, node->name(), _currentRoom->name()); // Bad node
            }
            else log->error(DGModControl, "%s", DGMsg230001);
            break;
    }
    
    if (_currentRoom) {
        if (_currentRoom->hasNodes()) {
            // Now we proceed to load the textures of the current node
            DGNode* currentNode = _currentRoom->currentNode();
            _textureManager->flush();
            
            if (currentNode->hasSpots()) {
                // Let the rest of the methods know they can loop the spots
                _canDrawSpots = true;
                
                currentNode->beginIteratingSpots();
                do {
                    DGSpot* spot = currentNode->currentSpot();
                    
                    if (spot->hasTexture())
                        _textureManager->requestTexture(spot->texture());
                    
                    if (spot->hasAudio()) {
                        DGAudio* audio = spot->audio();
                        
                        // Request the audio
                        audioManager->requestAudio(audio);
                        audio->setPosition(spot->face());
                        audio->setVolume(spot->volume());
                        
                        if (spot->hasFlag(DGSpotAuto))
                            audio->play();
                    }
                } while (currentNode->iterateSpots());
            }
            else {
                log->warning(DGModControl, "%s", DGMsg130001);
                _canDrawSpots = false;
            }
            
            // Prepare the name for the window
            char title[DGMaxObjectName];
            snprintf(title, DGMaxObjectName, "%s (%s, %s)", config->script(), 
                     _currentRoom->name(), currentNode->name());
            system->setTitle(title);
        }
        else _canDrawSpots = false;
        
        // This has to be done every time so that room audios keep playing
        if (!_currentRoom->hasAudios()) {
            vector<DGAudio*>::iterator it;
            vector<DGAudio*> arrayOfAudios = _currentRoom->arrayOfAudios();
            
            it = arrayOfAudios.begin();
            
            while (it != arrayOfAudios.end()) {
                audioManager->requestAudio((*it));
                (*it)->play();
                
                it++;
            }
        }
    }
    
    audioManager->flush();
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
    
    texture.bind();
    _render->copyScene();
    
    texture.saveToFile(buffer);   
    
    config->texCompression = previousTexCompression;
}

void DGControl::profiler() {    
    // Store the last FPS count and reset
    _fpsLastCount = _fpsCount;
    _fpsCount = 0;
}

void DGControl::terminate() {
    DGSystem::getInstance().terminate();
}

void DGControl::update() {
    // TODO: Suspend all operations when doing a switch
    // FIXME: Add a render stack of DGObjects, especially for overlays
    // FIXME: Rework all this
    
    // Setup the scene
    _render->resetScene();
    _render->clearScene();
    
    // Do this in a viewtimer
    switch (_state->current()) {
        case DGStateNode:
            // This is the first method we call because it sets the view
            _camera->update();
            
            // If the camera is rotating, check for spots under the cursor
            _scanSpots();
            
            if (!config->showSpots)
                _render->clearScene();
            
            // Drawing the node is handled by a separate function
            _drawSpots();
            
            break;
        case DGStateSplash:
            static int handlerIn = timerManager->createSimple(3);
            static int handlerOut = timerManager->createSimple(4);
            
            _camera->update();
            _camera->beginOrthoView();
            _render->beginDrawing(true);
            _render->drawSplash();
            _render->fadeScene();
            _render->endDrawing();
            _camera->endOrthoView();
        
            if (timerManager->check(handlerIn)) {
                
                _render->fadeOutNextUpdate();
            }
            
            if (timerManager->check(handlerOut)) {
                // FIXME: This is wrong because it overrides any changes made by the user
                //_render->fadeInNextUpdate();
                _render->resetFade();
                _state->set(DGStateNode);
                script->execute();
            }
            
            // Flush the buffers
            system->update();

            return;
            break;
    }
    
    // We now proceed with all the orthogonal projections,
    // standard to all states
    
    // TODO: Optimize all the begin / end drawing calls, especially for fonts
    _camera->beginOrthoView();
    
    // Blends, gamma, etc.
    _render->beginDrawing(true);
    _render->blendScene();
    _render->endDrawing();
    
    _drawInterface();
    
    // User post render operations, supporting textures
    _render->beginDrawing(true);
    if (_eventHandlers.hasPostRender)
        script->processCallback(_eventHandlers.postRender, 0);

    _render->fadeScene();
    _render->endDrawing();
    
    // Debug info, if enabled
    if (config->debugMode) {
        
        _fpsCount++;
        
        if (_console->isEnabled()) {
            _console->update();
            if (_console->isReadyToProcess()) {
                char command[DGMaxLogLength];
                _console->getCommand(command);
                script->processCommand(command);
            }
        }
        else {
            if (_console->isHidden()) {
                DGPoint position = cursorManager->position();
                
                _render->beginDrawing(true);
                // Set the color used for information
                _render->setColor(DGColorBrightCyan);
                _consoleFont->print(DGInfoMargin, DGInfoMargin, 
                                    "Viewport size: %d x %d", config->displayWidth, config->displayHeight);                
                _consoleFont->print(DGInfoMargin, (DGInfoMargin * 2) + DGDefFontSize, 
                                    "Coordinates: (%d, %d)", (int)position.x, (int)position.y);
                _consoleFont->print(DGInfoMargin, (DGInfoMargin * 3) + (DGDefFontSize * 2), 
                                    "Viewing angle: %2.1f", _camera->fieldOfView());
                _consoleFont->print(DGInfoMargin, (DGInfoMargin * 4) + (DGDefFontSize * 3), 
                                    "FPS: %d", _fpsLastCount); 
                _render->endDrawing();
            }
            else
                _console->update(); // Keep updating until done...
        }
        
    }
    
    _camera->endOrthoView();
    
    audioManager->setOrientation(_camera->orientation());
    timerManager->update();
    
    // Flush the buffers
    system->update();
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void DGControl::_drawInterface() {
    // Helpers
    _render->beginDrawing(false); // Textures disabled
    if (config->showHelpers) {
        if (_render->beginIteratingHelpers()) { // Check if we have any
            do {
                DGPoint point = _render->currentHelper();
                _render->setColor(DGColorBrightCyan);
                _render->drawHelper(point.x, point.y, true);
                
            } while (_render->iterateHelpers());
        }
    }
    _render->endDrawing();
    
    _render->beginDrawing(true);
    
    // Draw all active overlays
    _drawOverlays();
    
    // Feedback operations
    feedManager->update();
    
    // We do this because the feed manager might change color
    _render->setColor(DGColorWhite);
    
    // Mouse cursor
    if (cursorManager->hasImage()) { // A bitmap cursor is currently set
        cursorManager->bindImage();
        _render->drawSlide(cursorManager->arrayOfCoords());
    }
    else {
        DGPoint position = cursorManager->position();
        
        _render->endDrawing();
        _render->beginDrawing(false); // Textures disabled (only for default cursor)
        if (cursorManager->onButton() || cursorManager->hasAction())
            _render->setColor(DGColorBrightRed);
        else
            _render->setColor(DGColorDarkGray);
        _render->drawHelper(position.x, position.y, false);
    }
    
    _render->endDrawing();
}

void DGControl::_drawOverlays() {
    if (!_arrayOfOverlays.empty()) {
        vector<DGOverlay*>::iterator itOverlay;
        
        itOverlay = _arrayOfOverlays.begin();
        
        while (itOverlay != _arrayOfOverlays.end()) {
            if ((*itOverlay)->isEnabled()) {
                
                // Draw images first
                if ((*itOverlay)->hasImages()) {
                    (*itOverlay)->beginIteratingImages();
                    
                    do {
                        DGImage* image = (*itOverlay)->currentImage();
                        if (image->isEnabled()) {
                            image->update(); // Perform any necessary updates
                            image->texture()->bind();
                            _render->setAlpha(image->fadeLevel());
                            _render->drawSlide(image->arrayOfCoordinates());
                        }
                    } while ((*itOverlay)->iterateImages());
                }
                
                // Now buttons
                if ((*itOverlay)->hasButtons()) {
                    (*itOverlay)->beginIteratingButtons(false);
                    
                    do {
                        DGButton* button = (*itOverlay)->currentButton();
                        if (button->isEnabled()) {
                            if (button->hasTexture()) {
                                button->update();
                                _render->setAlpha(button->fadeLevel());
                                button->texture()->bind();
                                _render->drawSlide(button->arrayOfCoordinates());
                            }
                            
                            if (button->hasText()) {
                                DGPoint position = button->position();
                                _render->setColor(button->textColor());
                                button->font()->print(position.x, position.y, button->text());
                                _render->setColor(DGColorWhite); // Reset the color
                            }
                        }
                    } while ((*itOverlay)->iterateButtons());
                }
                
            }
            
            itOverlay++;
        }
    }    
}

void DGControl::_drawSpots() {
    // IMPORTANT: Ensure this function is thread-safe when
    // switching rooms or nodes
    
    if (_canDrawSpots) {
        DGNode* currentNode = _currentRoom->currentNode();
        if (currentNode->isEnabled()) {
            currentNode->update();
            _render->setAlpha(currentNode->fadeLevel());
            
            _render->beginDrawing(true);
            
            currentNode->beginIteratingSpots();
            do {
                DGSpot* spot = currentNode->currentSpot();
                
                if (spot->hasTexture() && spot->isEnabled()) {
                    spot->texture()->bind();
                    _render->drawPolygon(spot->arrayOfCoordinates(), spot->face());
                }
            } while (currentNode->iterateSpots());
            
            _render->endDrawing();
        }
    }
}

void DGControl::_processAction(){
    DGAction* action = cursorManager->action();
    
    switch (action->type) {
        case DGActionFunction:
            script->processCallback(action->luaHandler, action->luaObject);
            break;
        case DGActionFeed:
            feedManager->parse(action->feed);
            break;
        case DGActionSwitch:
            cursorManager->removeAction();
            switchTo(action->target);
            break;
    }
}

bool DGControl::_scanOverlays() {
    cursorManager->setOnButton(false);
    
    if (!_arrayOfOverlays.empty()) {
        vector<DGOverlay*>::reverse_iterator itOverlay;
        
        itOverlay = _arrayOfOverlays.rbegin();
        
        while (itOverlay != _arrayOfOverlays.rend()) {
            if ((*itOverlay)->isEnabled()) {
                
                if ((*itOverlay)->hasButtons()) {
                    (*itOverlay)->beginIteratingButtons(true);
                    
                    do {
                        DGButton* button = (*itOverlay)->currentButton();
                        if (button->isEnabled()) {
                            DGPoint position = cursorManager->position();
                            float* arrayOfCoordinates = button->arrayOfCoordinates();
                            if (position.x >= arrayOfCoordinates[0] && position.y >= arrayOfCoordinates[1] &&
                                position.x <= arrayOfCoordinates[4] && position.y <= arrayOfCoordinates[5]) {
                                
                                cursorManager->setOnButton(true);
                                if (button->hasAction())
                                    cursorManager->setAction(button->action());
                                
                                return true;
                            }
                        }
                    } while ((*itOverlay)->iterateButtons());
                }
            }
            
            itOverlay++;
        }
    }
    
    return false;
}

bool DGControl::_scanSpots() {
    // Check if the current node has spots to draw, and also if
    // we aren't dragging the view
    if (_canDrawSpots) {
        DGNode* currentNode = _currentRoom->currentNode();
        
        // Check if the current node is enabled
        if (currentNode->isEnabled()) {
            _render->beginDrawing(false);
        
            // First pass: draw the colored spots
            currentNode->beginIteratingSpots();
            do {
                DGSpot* spot = currentNode->currentSpot();
                
                if (spot->hasColor() && spot->isEnabled()) {
                    _render->setColor(spot->color());
                    _render->drawPolygon(spot->arrayOfCoordinates(), spot->face());
                }
            } while (currentNode->iterateSpots());
            
            // Second pass: test the color under the cursor and
            // set action, if available
            
            // FIXME: Should unify the checks here a bit more...
            if (!cursorManager->isDragging() && !_camera->isPanning() && !cursorManager->onButton()) {
                DGPoint position = cursorManager->position();
                cursorManager->removeAction();
                int color = _render->testColor(position.x, position.y);
                if (color) {
                    currentNode->beginIteratingSpots();
                    do {
                        DGSpot* spot = currentNode->currentSpot();
                        if (color == spot->color()) {
                            cursorManager->setAction(spot->action());
                            return true;
                        }
                    } while (currentNode->iterateSpots());
                }
            }
        
            _render->endDrawing();
        }
    }
    
    return false;
}
