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
    feedManager = &DGFeedManager::getInstance();
    fontManager = &DGFontManager::getInstance();
    log = &DGLog::getInstance();
    script = &DGScript::getInstance();
    system = &DGSystem::getInstance();
    timerManager = &DGTimerManager::getInstance();
	
	_mouseData.x = config->displayWidth / 2;
	_mouseData.y = config->displayHeight / 2;
	_mouseData.onButton = false;    
	_mouseData.onSpot = false;
    
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
    _render->updateScene(); // Test for errors
    
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
            system->terminate();
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
    if (_eventHandlers.hasMouseMove)
        script->processCallback(_eventHandlers.mouseMove, 0);
    
    if ((eventFlags & DGMouseEventUp) && _eventHandlers.hasMouseButton) {
        script->processCallback(_eventHandlers.mouseButton, 0);
    }
    
    // The overlay system is handled differently than the spots, so we
    // do everything here. Eventually, we should tide up things a little.
    
    // TODO: Merge test for action codes
    // TODO: Use active overlays only
    
    // TODO: Drag, start processing panning when mouse is down, stop when up event
    // FIXME: Buttons must be iterated from the end here

    _mouseData.onButton = false;
    if (!_arrayOfOverlays.empty()) {
        vector<DGOverlay*>::iterator itOverlay;
        
        itOverlay = _arrayOfOverlays.begin();
        
        while (itOverlay != _arrayOfOverlays.end()) {
            if ((*itOverlay)->isVisible()) {
                
                if ((*itOverlay)->hasButtons()) {
                    (*itOverlay)->beginIteratingButtons(true);
                    
                    do {
                        DGButton* button = (*itOverlay)->currentButton();
                        float* arrayOfCoordinates = button->arrayOfCoordinates();
                        if (_mouseData.x >= arrayOfCoordinates[0] && _mouseData.y >= arrayOfCoordinates[1] &&
                            _mouseData.x <= arrayOfCoordinates[4] && _mouseData.y <= arrayOfCoordinates[5]) {
                            _mouseData.onButton = true;
                            if ((eventFlags & DGMouseEventUp) && button->hasAction()) {
                                DGAction* action = button->action();
                                
                                switch (action->type) {
                                    case DGActionCustom:
                                        script->processCallback(action->luaHandler, button->luaObject());
                                        break;
                                    case DGActionFeed:
                                        feedManager->parse(action->feed);
                                        break;
                                    case DGActionSwitch:
                                        switchTo(action->target);
                                        break;
                                }
                                
                                // If the overlay containing the button is no longer visible,
                                // we force the corresponding cursor change.                                
                                if (!(*itOverlay)->isVisible()) {
                                    _mouseData.onButton = false;
                                   // isButtonPressed = false;
                                }
                                
                                // Should stop processing overlays
                                return;
                            }
                        }
                    } while ((*itOverlay)->iterateButtons());
                }
            }
            
            itOverlay++;
        }
    }
    
    // BUG: Stop processing clicks if button was found!!
    // BUG: If was dragging, also stop processing
    // Previous state, etc.
    
    // Proceed with spot detection
    
    if ((eventFlags & DGMouseEventUp) && _mouseData.onSpot && !_mouseData.isDragging) {
        // We can safely assume that all data is in place now,
        // so we execute the action
        DGNode* currentNode = _currentRoom->currentNode();
        
        currentNode->beginIteratingSpots();
        do {
            DGSpot* spot = currentNode->currentSpot();
            
            if (spot->hasColor() && (spot->color() == _mouseData.color)) {
                DGAction* action = spot->action();
                
                switch (action->type) {
                    case DGActionCustom:
                        script->processCallback(action->luaHandler, spot->luaObject());
                        break;
                    case DGActionFeed:
                        feedManager->parse(action->feed);
                        break;
                    case DGActionSwitch:
                        // As a precaution, we reset the onSpot flag
                        _mouseData.onSpot = false;
                        switchTo(action->target);
                        break;
                }
                
                break;
            }
        } while (currentNode->iterateSpots());
    }
    
    if ((eventFlags & DGMouseEventUp) && _mouseData.isDragging) {
        _mouseData.isDragging = false;
        _camera->stopDragging();
    }
    
    if ((eventFlags & DGMouseEventMove)) {
        _mouseData.x = x;
        _mouseData.y = y;
    }
    
    if ((eventFlags & DGMouseEventDown)) {
        _mouseData.x = x;
        _mouseData.y = y;
        _camera->startDragging(_mouseData.x, _mouseData.y);
    }
    
    if ((eventFlags & DGMouseEventDrag) && !_mouseData.isDragging) { 
        _mouseData.isDragging = true;
        _mouseData.x = x;
        _mouseData.y = y;
    }
    
    switch (config->controlMode) {
        case DGMouseFree:
            _mouseData.x = x;
            _mouseData.y = y;
            if (!_mouseData.onButton)
                _camera->pan(_mouseData.x, _mouseData.y);
            else _camera->stopDragging();
            break;
            
        case DGMouseDrag:
            if ((eventFlags & DGMouseEventDrag) && _mouseData.isDragging) {
                _mouseData.x = x;
                _mouseData.y = y;      
                _camera->pan(_mouseData.x, _mouseData.y);
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

void DGControl::update() {
    // TODO: Suspend all operations when doing a switch
    // FIXME: Add a render stack of DGObjects, especially for overlays
    
    // Setup the scene
    _render->resetScene();
    _render->clearScene();
    
    // Do this in a viewtimer
    switch (_state->current()) {
        case DGStateNode:
            // This is the first method we call because it sets the view
            _camera->update();
            
            _scanSpots();
            
            if (!config->showSpots)
                _render->clearScene();
            
            // Drawing the node is handled by a separate function
            _drawScene();
            
            break;
    }
    
    // We now proceed with all the orthogonal projections,
    // standard to all states
    
    // TODO: Optimize all the begin / end drawing calls, especially for fonts
    _camera->beginOrthoView();
    
    // Blends, gamma, etc.
    _render->updateScene();
    
    // Overlays
    // TODO: Move into a separate function
    
    if (!_arrayOfOverlays.empty()) {
        _render->beginDrawing(true);
        
        vector<DGOverlay*>::iterator itOverlay;
        
        itOverlay = _arrayOfOverlays.begin();
        
        while (itOverlay != _arrayOfOverlays.end()) {
            if ((*itOverlay)->isVisible()) {
                
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
                
            }
            
            itOverlay++;
        }
        
        _render->endDrawing();
    }
    
    // Feedback
    _render->beginDrawing(true);
    feedManager->update();
    _render->endDrawing();
    
    // Helpers
    if (config->showHelpers) {
        if (_render->beginIteratingHelpers()) { // Check if we have any
            do {
                DGPoint point = _render->currentHelper();
                
                _render->beginDrawing(false); // Textures disabled
                _render->setColor(DGColorBrightCyan);
                _render->drawHelper(point.x, point.y, true);
                _render->endDrawing();
            } while (_render->iterateHelpers());
        }
    }
    
    // Mouse cursor
    _render->beginDrawing(false); // Textures disabled (only for default cursor)
    if (_mouseData.onButton || _mouseData.onSpot)
        _render->setColor(DGColorBrightRed);
    else
        _render->setColor(DGColorDarkGray);
    _render->drawHelper(_mouseData.x, _mouseData.y, false);
    _render->endDrawing();
    
    // User post render operations, supporting textures
    _render->beginDrawing(true);
    if (_eventHandlers.hasPostRender)
        script->processCallback(_eventHandlers.postRender, 0);
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
                _render->beginDrawing(true);
                // Set the color used for information
                _render->setColor(DGColorBrightCyan);
                _consoleFont->print(DGInfoMargin, DGInfoMargin, 
                                    "Viewport size: %d x %d", config->displayWidth, config->displayHeight);                
                _consoleFont->print(DGInfoMargin, (DGInfoMargin * 2) + DGDefFontSize, 
                                    "Coordinates: (%d, %d)", _mouseData.x, _mouseData.y);
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

void DGControl::_drawScene() {
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

void DGControl::_scanOverlays() {
}

void DGControl::_scanSpots() {
    // TODO: To make this function more efficient, we should
    // stop drawing once a spot has been detected
    
    if (_canDrawSpots) {
        DGNode* currentNode = _currentRoom->currentNode();

        _render->beginDrawing(false);
    
        currentNode->beginIteratingSpots();
        do {
            DGSpot* spot = currentNode->currentSpot();
            
            if (spot->hasColor() && spot->isEnabled()) {
                _render->setColor(spot->color());
                _render->drawPolygon(spot->arrayOfCoordinates(), spot->face());
            }
        } while (currentNode->iterateSpots());
        
        if ((_mouseData.color = _render->testColor(_mouseData.x, _mouseData.y)))
            _mouseData.onSpot = true;
        else _mouseData.onSpot = false;
    
        _render->endDrawing();
    }
}
