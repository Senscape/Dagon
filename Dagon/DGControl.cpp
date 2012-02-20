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

#include "DGCamera.h"
#include "DGConfig.h"
#include "DGControl.h"
#include "DGFeedManager.h"
#include "DGFont.h"
#include "DGLog.h"
#include "DGNode.h"
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
    log = &DGLog::getInstance();
    config = &DGConfig::getInstance();
    script = &DGScript::getInstance();
    system = &DGSystem::getInstance();
    timerManager = &DGTimerManager::getInstance();
	
	_mouseData.x = config->displayWidth / 2;
	_mouseData.y = config->displayHeight / 2;
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
        delete _feedManager;
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
    _render->updateScene();
    
    _camera = new DGCamera;
    _camera->setViewport(config->displayWidth, config->displayHeight);
    
    // Load the default font with a small height
    _defaultFont = new DGFont;
    _defaultFont->init();
    _defaultFont->setDefault(DGDefFontSize);
    
    _feedManager = new DGFeedManager;
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

void DGControl::processKey(int aKey, bool isModified) {
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
		case DGKeyEsc:
            system->terminate();
			break;
		case DGKeyQuote:
		case DGKeyTab:         
			//DGConsoleToggleState();
			break;
		case DGKeyBackspace:
			//DGConsoleDeleteChar();
			break;
		case DGKeyEnter:
			//DGConsoleExecute();
			break;
        case 'f':
        case 'F':
            if (isModified) {
                system->toggleFullScreen();
            }
            //else DGConsoleInputChar(aKey);
            break;
		default:
			//DGConsoleInputChar(aKey);
			break;            
	}
	
    if (idx) {
        //if (_hotKeyData[idx].enabled)
            //DGScriptDoLine(hotkey[idx].line);
    }
}

void DGControl::processMouse(int x, int y, bool isButtonPressed) {
    if (isButtonPressed && _mouseData.onSpot) {
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
                        script->callback(action->luaHandler, spot->luaObject());
                        break;
                    case DGActionFeed:
                        _feedManager->parse(action->feed);
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
    
    _mouseData.x = x;
    _mouseData.y = y;
    _camera->pan(_mouseData.x, _mouseData.y);
}

void DGControl::registerGlobalHandler(int forEvent, int handlerForLua) {
    switch (forEvent) {
		case DGEventEnterNode:
			_eventsLuaFunctions.enterNode = handlerForLua;
			_eventsLuaFunctions.hasEnterNode = true;
			break;
		case DGEventLeaveNode:
			_eventsLuaFunctions.leaveNode = handlerForLua;
			_eventsLuaFunctions.hasLeaveNode = true;
			break;
		case DGEventEnterRoom:
			_eventsLuaFunctions.enterRoom = handlerForLua;
			_eventsLuaFunctions.hasEnterRoom = true;
			break;
		case DGEventLeaveRoom:
			_eventsLuaFunctions.leaveRoom = handlerForLua;
			_eventsLuaFunctions.hasLeaveRoom = true;
			break;
		case DGEventPreRender:
			_eventsLuaFunctions.preRender = handlerForLua;
			_eventsLuaFunctions.hasPreRender = true;
			break;
		case DGEventPostRender:
			_eventsLuaFunctions.postRender = handlerForLua;
			_eventsLuaFunctions.hasPostRender = true;
			break;
		case DGEventMouseButton:
			_eventsLuaFunctions.mouseButton = handlerForLua;
			_eventsLuaFunctions.hasMouseButton = true;
			break;			
		case DGEventMouseMove:
			_eventsLuaFunctions.mouseMove = handlerForLua;
			_eventsLuaFunctions.hasMouseMove = true;
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
             _textureManager->requestNewBundle((DGNode*)theTarget);
            break;
        case DGObjectRoom: 
            _arrayOfRooms.push_back((DGRoom*)theTarget);
            break;
    }
}

int DGControl::registerTimer(double trigger, int handlerForLua) {
    return timerManager->create(trigger, handlerForLua);
}

void DGControl::reshape(int width, int height) {
    config->displayWidth = width;
    config->displayHeight = height;
    
    _camera->setViewport(width, height);
}

void DGControl::showFeed(const char* text) {
    _feedManager->parse(text);
}

void DGControl::sleep(int forMilliseconds) {
    
}

void DGControl::switchTo(DGObject* theTarget) {
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
                        _textureManager->requireTextureToLoad(spot->texture());
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
    }
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
    
    // Setup the scene
    _render->clearScene();
    
    // Do this in a viewtimer
    switch (_state->current()) {
        case DGStateNode:
            // This is the first method we call because it sets the view
            _camera->update();
            
            // Drawing the node is handled by a separate function
            _drawScene();
            
            // We now proceed with all the orthogonal projections
            _camera->beginOrthoView();
            
            if (_feedManager->isQueued()) {
                DGPoint location = _feedManager->location();
                _render->setColor(_feedManager->color());
                _defaultFont->print(location.x, location.y, _feedManager->text());
            }
                
            _render->beginDrawing(false);
            if (_mouseData.onSpot)
                _render->setColor(DGColorBrightRed);
            else
                _render->setColor(DGColorDarkGray);
            _render->drawCursor(_mouseData.x, _mouseData.y);
            _render->endDrawing();
            
            if (config->debugMode) {
                // Set the color used for information
                _render->setColor(DGColorBrightCyan);
                _defaultFont->print(DGInfoMargin, DGInfoMargin, 
                                    "Viewport size: %d x %d", config->displayWidth, config->displayHeight);                
                _defaultFont->print(DGInfoMargin, (DGInfoMargin * 2) + DGDefFontSize, 
                                    "Coordinates: (%d, %d)", _mouseData.x, _mouseData.y);
                _defaultFont->print(DGInfoMargin, (DGInfoMargin * 3) + (DGDefFontSize * 2), 
                                    "Viewing angle: %2.1f", _camera->fieldOfView());
                _defaultFont->print(DGInfoMargin, (DGInfoMargin * 4) + (DGDefFontSize * 3), 
                                    "FPS: %d", _fpsLastCount);                
            }
            
            _camera->endOrthoView();
            
            break;
    }
    
    timerManager->update();
    _feedManager->update();
    
    // Flush the buffers
    system->update();
    
    if (config->debugMode) {
        _fpsCount++;
    }
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void DGControl::_drawScene() {
    // IMPORTANT: Ensure this function is thread-safe when
    // switching rooms or nodes
    
    _scanSpots();
    
    if (_canDrawSpots) {
        DGNode* currentNode = _currentRoom->currentNode();
        
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
    
    // FIXME: This should go between begin and endOrthoView calls because
    // the blending texture is drawn here
    _render->updateScene();
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
