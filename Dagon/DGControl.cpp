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
    render = &DGRenderManager::getInstance();    
    script = &DGScript::getInstance();
    system = &DGSystem::getInstance();
    timerManager = &DGTimerManager::getInstance();
    videoManager = &DGVideoManager::getInstance();
    
    _currentRoom = NULL;
    
    _fpsCount = 0;
    
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
    
    render->init();
    render->resetView(); // Test for errors

    cameraManager->setViewport(config->displayWidth, config->displayHeight);
    
    // Init the audio manager
    audioManager->init();
    
    // Init the font library
    fontManager->init();
    
    // Init the video manager
    videoManager->init();
    
    _console = new DGConsole;
    if (config->debugMode) {
        // Console must be initialized after the Font Manager
        _console->init();
        _console->enable();
    }
    
    feedManager->init();
    
    _interface = new DGInterface;
    _scene = new DGScene;
    
    _state = new DGState;
    _textureManager = new DGTextureManager;
    
    _isInitialized = true;
    
    if (config->debugMode)
        _console->enable();
    
    // If the splash screen is enabled, load its data and set the correct state
    if (config->showSplash) {
        _scene->loadSplash();
        _state->set(DGStateSplash);
        render->fadeInNextUpdate();
    }
    else render->resetFade();
    
    ///////////////////////////
    
   /* DGVideo video;
    video.setResource(config->path(DGPathRes, "vid_mad_scare.ogv"));
    video.load();
    video.play();
    video.update();
    
    DGFrame* frame = video.currentFrame();
    DGTexture texture;
    texture.loadRawData(frame->data, frame->width, frame->height);
    texture.saveToFile("test");*/
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
            _scene->fadeOut();
            timerManager->createInternal(1, terminate);
			break;
		case DGKeyQuote:
		case DGKeyTab:         
			_console->toggle();
			break;
		case DGKeySpacebar:
            if (_console->isHidden()) 
                config->showHelpers = !config->showHelpers;
			break;
        case 'f':
        case 'F':
            if (isModified) system->toggleFullScreen();
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
}

void DGControl::processMouse(int x, int y, int eventFlags) {
    if (!cursorManager->isEnabled()) // Ignore everything
        return;
            
    if ((eventFlags & DGMouseEventMove) && (_eventHandlers.hasMouseMove))
        script->processCallback(_eventHandlers.mouseMove, 0);
    
    if ((eventFlags & DGMouseEventUp) && _eventHandlers.hasMouseButton) {
        script->processCallback(_eventHandlers.mouseButton, 0);
    }
    
    // The overlay system is handled differently than the spots, so we
    // do everything here. Eventually, we should tide up things a little.

    // TODO: Use active overlays only
    if (!cursorManager->isDragging()) {
        if (_interface->scanOverlays()) {
            if ((eventFlags & DGMouseEventUp) && cursorManager->hasAction()) {
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
            if (eventFlags & DGMouseEventMove) {
                if (!cursorManager->onButton()) {
                    cameraManager->pan(x, y);
                    
                    // When in free mode, we first check if camera is panning because
                    // we don't want the "not dragging" cursor
                    if (cameraManager->isPanning()) { 
                        cursorManager->setCursor(cameraManager->cursorWhenPanning());
                    }
                }
                else cameraManager->stopPanning();
            }
            
            if (eventFlags & DGMouseEventUp) {
                if (cursorManager->hasAction())
                    _processAction();
            }
            break;
            
        case DGMouseDrag:
            if (eventFlags & DGMouseEventDrag) {
                if (cursorManager->isDragging()) {
                    cameraManager->pan(x, y);
                    cursorManager->setCursor(cameraManager->cursorWhenPanning());
                }
                else cursorManager->setDragging(true);
            }
            
            // FIXME: Start dragging a few milliseconds after the mouse if down
            if (eventFlags & DGMouseEventDown) {
                cameraManager->startDragging(x, y);
            }
            
            if (eventFlags & DGMouseEventUp) {
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
            _interface->addOverlay((DGOverlay*)theTarget);
            break;
        case DGObjectRoom: 
            _arrayOfRooms.push_back((DGRoom*)theTarget);
            break;
    }
}

void DGControl::reshape(int width, int height) {
    config->displayWidth = width;
    config->displayHeight = height;
    
    cameraManager->setViewport(width, height);
    
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
        render->clearView();
        cameraManager->update();
        _scene->drawSpots();
        render->blendNextUpdate();
        cameraManager->simulateWalk();
        ///////////////////////////
    }
    else firstSwitch = false;
    
    audioManager->clear();
    system->suspendManager();
    videoManager->flush();
    system->resumeManager();
    
    switch (theTarget->type()) {
        case DGObjectRoom:
            _currentRoom = (DGRoom*)theTarget;
            _scene->setRoom((DGRoom*)theTarget);
            
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
                currentNode->beginIteratingSpots();
                do {
                    DGSpot* spot = currentNode->currentSpot();
                    
                    if (spot->hasAudio()) {
                        DGAudio* audio = spot->audio();
                        
                        // Request the audio
                        audioManager->requestAudio(audio);
                        audio->setPosition(spot->face());
                        audio->setVolume(spot->volume());
                        
                        if (spot->hasFlag(DGSpotAuto))
                            audio->play();
                    }
                    
                    if (spot->hasVideo()) {
                        DGVideo* video = spot->video();
                        video->load();
                        
                        if (video->isLoaded()) {
                            // Do this here?
                            video->play();
                            video->update();
                            videoManager->requestVideo(video);
                            ////////////////
                            
                            if (!spot->hasTexture()) {
                                DGFrame* frame = video->currentFrame();
                                DGTexture* texture = new DGTexture;
                                texture->loadRawData(frame->data, frame->width, frame->height);
                                spot->setTexture(texture);
                            }
                        }
                    }
                    
                    if (spot->hasTexture()) {
                        _textureManager->requestTexture(spot->texture());
                        
                        // Only resize if nothing but origin
                        if (spot->vertexCount() == 1)
                            spot->resize(spot->texture()->width(), spot->texture()->height());
                    }
                } while (currentNode->iterateSpots());
            }
            else {
                log->warning(DGModControl, "%s", DGMsg130001);
            }
            
            // Prepare the name for the window
            char title[DGMaxObjectName];
            snprintf(title, DGMaxObjectName, "%s (%s, %s)", config->script(), 
                     _currentRoom->name(), currentNode->name());
            system->setTitle(title);
        }
        
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
    render->copyView();
    
    texture.saveToFile(buffer);   
    
    config->texCompression = previousTexCompression;
}

void DGControl::profiler() {    
    // Store the last FPS count and reset
    config->setFramesPerSecond(_fpsCount);
    _fpsCount = 0;
}

void DGControl::terminate() {
    DGSystem::getInstance().terminate();
}

void DGControl::update() {
    // TODO: Suspend all operations when doing a switch
    // FIXME: Add a render stack of DGObjects, especially for overlays
    // IMPORTANT: Ensure this function is thread-safe when
    // switching rooms or nodes
    
    // Setup the scene
    _scene->clear();
    
    // Do this in a viewtimer
    switch (_state->current()) {
        case DGStateNode:
            _scene->scanSpots();
            _scene->drawSpots();
            _interface->drawHelpers();
            _interface->drawOverlays();
            feedManager->update();
            _interface->drawCursor();
            
            break;
        case DGStateSplash:
            static int handlerIn = timerManager->createManual(3);
            static int handlerOut = timerManager->createManual(4);
            
            _scene->drawSplash();

            if (timerManager->checkManual(handlerIn)) {
                _scene->fadeOut();
            }
            
            if (timerManager->checkManual(handlerOut)) {
                render->clearView();
                render->resetFade();
                _state->set(DGStateNode);
                _scene->unloadSplash();
                script->execute();
            }

            break;
    }
    
    // User post render operations, supporting textures
    if (_eventHandlers.hasPostRender)
        script->processCallback(_eventHandlers.postRender, 0);

    // General fade, affects every graphic on screen
    render->fadeView();
    
    // Debug info, if enabled
    if (_console->isEnabled()) {
        _fpsCount++;
        _console->update();
        
        if (_console->isReadyToProcess()) {
            char command[DGMaxLogLength];
            _console->getCommand(command);
            script->processCommand(command);
        }
    }

    cameraManager->endOrthoView();
    
    audioManager->setOrientation(cameraManager->orientation());
    timerManager->process();
    
    // Flush the buffers
    system->update();
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
            feedManager->parse(action->feed);
            break;
        case DGActionSwitch:
            cursorManager->removeAction();
            switchTo(action->target);
            break;
    }
}
