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

#include "DGCameraManager.h"
#include "Config.h"
#include "DGCursorManager.h"
#include "Font.h"
#include "Node.h"
#include "DGRenderManager.h"
#include "Room.h"
#include "DGScene.h"
#include "Spot.h"
#include "Texture.h"
#include "DGVideoManager.h"

#include "DGState.h"

using namespace std;

DGVideo _cutscene; // FIXME: Static to avoid Theora issues, revise later

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

// TODO: Improve rendering of layers supporting active layers
DGScene::DGScene() :
    cameraManager(DGCameraManager::instance()),
    config(Config::instance()),
    cursorManager(DGCursorManager::instance()),
    renderManager(DGRenderManager::instance()),
    videoManager(DGVideoManager::instance())
{
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGScene::~DGScene() {
    if (_isCutsceneLoaded)
        this->unloadCutscene();
    
    if (_isSplashLoaded)
        this->unloadSplash();
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGScene::clear() {
    renderManager.resetView();
    renderManager.clearView();
    
    // This is the first method we call because it sets the view
    cameraManager.update();
}

void DGScene::drawSpots() {
    bool processed = false;
    
    if (_canDrawSpots) {
        Node* currentNode = _currentRoom->currentNode();
        if (currentNode->isEnabled()) {
            renderManager.enablePostprocess();
            renderManager.clearView();
            
            currentNode->updateFade();
            renderManager.setAlpha(currentNode->fadeLevel());
            
            currentNode->beginIteratingSpots();
            do {
                Spot* spot = currentNode->currentSpot();
                
                if (spot->hasTexture() && spot->isEnabled()) {
                    
                    if (spot->hasVideo()) {
                        // If it has a video, we need to check if it's playing
                        if (spot->isPlaying()) { // FIXME: Must stop the spot later!
                            if (spot->video()->hasNewFrame()) {
                                DGFrame* frame = spot->video()->currentFrame();
                                Texture* texture = spot->texture();
                                texture->loadRawData(frame->data, frame->width, frame->height);
                            }
                            
                            spot->texture()->bind();
                            renderManager.drawPolygon(spot->arrayOfCoordinates(), spot->face());
                        }
                    }
                    else {
                        // Draw right away...
                        spot->texture()->bind();
                        renderManager.drawPolygon(spot->arrayOfCoordinates(), spot->face());
                    }
                }
            } while (currentNode->iterateSpots());
            
            if (config.showSpots) {
                renderManager.disableTextures();
                
                currentNode->beginIteratingSpots();
                do {
                    Spot* spot = currentNode->currentSpot();
                    
                    if (spot->hasColor() && spot->isEnabled()) {
                        renderManager.setColor(0x2500AAAA);
                        renderManager.drawPolygon(spot->arrayOfCoordinates(), spot->face());
                    }
                } while (currentNode->iterateSpots());
                
                renderManager.enableTextures();
            }
            
            renderManager.disablePostprocess();
            processed = true;
        }
    }
    
    
    // Blends, gamma, etc.
    cameraManager.beginOrthoView();
    if (processed)
        renderManager.drawPostprocessedView();
    renderManager.blendView();
}

void DGScene::fadeIn() {
    renderManager.fadeInNextUpdate();
}

void DGScene::fadeOut() {
    renderManager.fadeOutNextUpdate();
}

bool DGScene::scanSpots() {
    bool foundAction = false;
    
    // Check if the current node has spots to draw, and also if
    // we aren't dragging the view
    if (_canDrawSpots) {
        Node* currentNode = _currentRoom->currentNode();
        
        // Check if the current node is enabled
        if (currentNode->isEnabled()) {
            renderManager.disableAlpha();
            renderManager.disableTextures();
            
            // First pass: draw the colored spots
            currentNode->beginIteratingSpots();
            do {
                Spot* spot = currentNode->currentSpot();
                
                if (spot->hasColor() && spot->isEnabled()) {
                    renderManager.setColor(spot->color());
                    renderManager.drawPolygon(spot->arrayOfCoordinates(), spot->face());
                }
            } while (currentNode->iterateSpots());
            
            // Second pass: test the color under the cursor and
            // set action, if available
            
            // FIXME: Should unify the checks here a bit more...
            if (!cursorManager.isDragging() && !cursorManager.onButton()) {
                Point position = cursorManager.position();
                int color = renderManager.testColor(position.x, position.y);
                if (color) {
                    currentNode->beginIteratingSpots();
                    do {
                        Spot* spot = currentNode->currentSpot();
                        if (color == spot->color()) {
                            cursorManager.setAction(spot->action());
                            foundAction = true;
                            
                            break;
                        }
                    } while (currentNode->iterateSpots());
                }
                
                if (!foundAction) {
                    cursorManager.removeAction();
                    
                    if (cameraManager.isPanning())
                        cursorManager.setCursor(cameraManager.cursorWhenPanning());
                    else cursorManager.setCursor(kCursorNormal);
                }
            }
            
            renderManager.enableAlpha();
            renderManager.enableTextures();
        }
    }
    
    renderManager.clearView();
    
    if (foundAction) return true;
    else return false;
}

void DGScene::setRoom(Room* room) {
    _currentRoom = room;
    
    // Determine if spots can be drawn
    Node* node = _currentRoom->currentNode();
    
    if (node) {
        if (node->hasSpots())
            _canDrawSpots = true;
        else
            _canDrawSpots = false;
    }
    else _canDrawSpots = false;
}

////////////////////////////////////////////////////////////
// Implementation - Cutscene operations
////////////////////////////////////////////////////////////

void DGScene::cancelCutscene() {
  _cutscene.stop();
}

bool DGScene::drawCutscene() {
    if (_cutscene.isPlaying()) {
        if (_cutscene.hasNewFrame()) {
            DGFrame* frame = _cutscene.currentFrame();
            _cutsceneTexture->loadRawData(frame->data, frame->width, frame->height);
        }
        
        _cutsceneTexture->bind();
        
        // Note this is inverted
        int coords[] = {0, 0,
            config.displayWidth, 0,
            config.displayWidth, config.displayHeight,
            0, config.displayHeight};
        
        renderManager.enablePostprocess();
        cameraManager.beginOrthoView();
        renderManager.enableTextures();
        renderManager.drawSlide(coords);
        renderManager.disablePostprocess();
        renderManager.drawPostprocessedView();
        
        return true;
    }
    
    return false;
}

void DGScene::loadCutscene(const char* fileName) {
    _cutsceneTexture = new Texture;
    
    _cutscene.setResource(config.path(kPathResources, fileName, kObjectVideo).c_str());
    videoManager.requestVideo(&_cutscene);
    
    if (_cutscene.isLoaded()) {
        _cutscene.play();
        
        DGFrame* frame = _cutscene.currentFrame();
        _cutsceneTexture->loadRawData(frame->data, frame->width, frame->height);

        _isCutsceneLoaded = true;
    }
}

void DGScene::unloadCutscene() {
    _cutscene.unload();
    
    _cutsceneTexture->unload();
    delete _cutsceneTexture;
    
    _isCutsceneLoaded = false;
}

////////////////////////////////////////////////////////////
// Implementation - Splash screen operations
////////////////////////////////////////////////////////////

void DGScene::drawSplash() {
    _splashTexture->bind();
    
    // Note this is inverted
    int coords[] = {0, 0,
        config.displayWidth, 0,
        config.displayWidth, config.displayHeight,
        0, config.displayHeight}; 
    
    cameraManager.beginOrthoView();
    renderManager.enableTextures();
    renderManager.drawSlide(coords);
}

void DGScene::loadSplash() {
    _splashTexture = new Texture;
    _splashTexture->loadFromMemory(kDefSplashBinary, 179608);
    _isSplashLoaded = true;
}

void DGScene::unloadSplash() {
    _splashTexture->unload();
    delete _splashTexture;
    _isSplashLoaded = false;
}
