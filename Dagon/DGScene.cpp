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

#include "DGCameraManager.h"
#include "DGConfig.h"
#include "DGCursorManager.h"
#include "DGFont.h"
#include "DGNode.h"
#include "DGRenderManager.h"
#include "DGRoom.h"
#include "DGScene.h"
#include "DGSpot.h"
#include "DGTexture.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

// TODO: Improve rendering of layers supporting active layers
DGScene::DGScene() {
    cameraManager = &DGCameraManager::getInstance(); 
    config = &DGConfig::getInstance();  
    cursorManager = &DGCursorManager::getInstance();
    renderManager = &DGRenderManager::getInstance();
    
    _canDrawSpots = false;
    _isSplashLoaded = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGScene::~DGScene() {
    if (_isSplashLoaded)
        this->unloadSplash();
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGScene::clear() {
    renderManager->resetView();
    renderManager->clearView();
    
    // This is the first method we call because it sets the view
    cameraManager->update();
}

void DGScene::drawSpots() {
    if (_canDrawSpots) {
        DGNode* currentNode = _currentRoom->currentNode();
        if (currentNode->isEnabled()) {
            currentNode->update();
            renderManager->setAlpha(currentNode->fadeLevel());
            
            currentNode->beginIteratingSpots();
            do {
                DGSpot* spot = currentNode->currentSpot();
                
                if (spot->hasTexture() && spot->isEnabled()) {
                    spot->texture()->bind();
                    renderManager->drawPolygon(spot->arrayOfCoordinates(), spot->face());
                }
            } while (currentNode->iterateSpots());
        }
    }
    
    // Blends, gamma, etc.
    cameraManager->beginOrthoView();
    renderManager->blendView();
}

void DGScene::fadeIn() {
    renderManager->fadeInNextUpdate();
}

void DGScene::fadeOut() {
    renderManager->fadeOutNextUpdate();
}

bool DGScene::scanSpots() {
    bool foundAction = false;
    
    // Check if the current node has spots to draw, and also if
    // we aren't dragging the view
    if (_canDrawSpots) {
        DGNode* currentNode = _currentRoom->currentNode();
        
        // Check if the current node is enabled
        if (currentNode->isEnabled()) {
            renderManager->disableAlpha();
            renderManager->disableTextures();
            
            // First pass: draw the colored spots
            currentNode->beginIteratingSpots();
            do {
                DGSpot* spot = currentNode->currentSpot();
                
                if (spot->hasColor() && spot->isEnabled()) {
                    renderManager->setColor(spot->color());
                    renderManager->drawPolygon(spot->arrayOfCoordinates(), spot->face());
                }
            } while (currentNode->iterateSpots());
            
            // Second pass: test the color under the cursor and
            // set action, if available
            
            // FIXME: Should unify the checks here a bit more...
            if (!cursorManager->isDragging() && !cameraManager->isPanning() && !cursorManager->onButton()) {
                DGPoint position = cursorManager->position();
                cursorManager->removeAction();
                int color = renderManager->testColor(position.x, position.y);
                if (color) {
                    currentNode->beginIteratingSpots();
                    do {
                        DGSpot* spot = currentNode->currentSpot();
                        if (color == spot->color()) {
                            cursorManager->setAction(spot->action());
                            foundAction = true;
                            
                            break;
                        }
                    } while (currentNode->iterateSpots());
                }
            }
            
            renderManager->enableAlpha();
            renderManager->enableTextures();
        }
    }
    
    if (!config->showSpots)
        renderManager->clearView();
    else {
        // FIXME: This is a hack to show the spots, fix later
        glBlendFunc(GL_ONE, GL_ONE);
        renderManager->setAlpha(1.0f);   
    }
    
    if (foundAction) return true;
    else return false;
}

void DGScene::setRoom(DGRoom* room) {
    _currentRoom = room;
    
    // Determine if spots can be drawn
    DGNode* node = _currentRoom->currentNode();
    
    if (node->hasSpots())
        _canDrawSpots = true;
    else
        _canDrawSpots = false;
}

////////////////////////////////////////////////////////////
// Implementation - Splash screen operations
////////////////////////////////////////////////////////////

void DGScene::drawSplash() {
    _splashTexture->bind();
    
    // Note this is inverted
    float coords[] = {0, config->displayHeight, 
        config->displayWidth, config->displayHeight, 
        config->displayWidth, 0,
        0, 0}; 
    
    cameraManager->beginOrthoView();
    renderManager->enableTextures();
    renderManager->drawSlide(coords);
}

void DGScene::loadSplash() {
    _splashTexture = new DGTexture;
    _splashTexture->loadFromMemory(DGDefSplashBinary);
    _isSplashLoaded = true;
}

void DGScene::unloadSplash() {
    _splashTexture->unload();
    delete _splashTexture;
    _isSplashLoaded = false;
}
