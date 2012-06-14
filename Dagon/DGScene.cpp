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

#include "DGButton.h"
#include "DGCamera.h"
#include "DGConfig.h"
#include "DGCursorManager.h"
#include "DGFont.h"
#include "DGImage.h"
#include "DGLog.h"
#include "DGNode.h"
#include "DGOverlay.h"
#include "DGRender.h"
#include "DGRoom.h"
#include "DGScene.h"
#include "DGSpot.h"
#include "DGTexture.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGScene::DGScene() {
    config = &DGConfig::getInstance();  
    cursorManager = &DGCursorManager::getInstance();
    log = &DGLog::getInstance();
    
    _camera = new DGCamera;
    _camera->setViewport(config->displayWidth, config->displayHeight);
    
    _render = new DGRender;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGScene::~DGScene() {
    delete _camera;
    delete _render;
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGScene::addOverlay(DGOverlay* overlay) {
    _arrayOfOverlays.push_back(overlay);
}

void DGScene::clear() {
    _render->resetView();
    _render->clearView();
}

void DGScene::drawCursor() {
    // Mouse cursor
    if (cursorManager->hasImage()) { // A bitmap cursor is currently set
        cursorManager->bindImage();
        _render->drawSlide(cursorManager->arrayOfCoords());
    }
    else {
        DGPoint position = cursorManager->position();
        
        _render->disableTextures(); // Default cursor doesn't require textures
        if (cursorManager->onButton() || cursorManager->hasAction())
            _render->setColor(DGColorBrightRed);
        else
            _render->setColor(DGColorDarkGray);
        _render->drawHelper(position.x, position.y, false);
    }
}

void DGScene::drawHelpers() {
    // Helpers
    _render->disableTextures();
    if (config->showHelpers) {
        if (_render->beginIteratingHelpers()) { // Check if we have any
            do {
                DGPoint point = _render->currentHelper();
                _render->setColor(DGColorBrightCyan);
                _render->drawHelper(point.x, point.y, true);
                
            } while (_render->iterateHelpers());
        }
    }
    
    _render->enableTextures();
}

void DGScene::drawOverlays() {
    if (!_arrayOfOverlays.empty()) {
        vector<DGOverlay*>::iterator itOverlay;
        
        itOverlay = _arrayOfOverlays.begin();
        
        while (itOverlay != _arrayOfOverlays.end()) {
            if ((*itOverlay)->isEnabled()) {
                
                // Draw buttons first
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
                
                // Now images
                // NOTE: This isn't quite right, images should be drawn first. Perhaps
                // allow the user to setup the order for each overlay.
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
    }   
}

void DGScene::drawSplash() {
    
}

void DGScene::drawSpots() {
    if (_canDrawSpots) {
        DGNode* currentNode = _currentRoom->currentNode();
        if (currentNode->isEnabled()) {
            currentNode->update();
            _render->setAlpha(currentNode->fadeLevel());
            
            currentNode->beginIteratingSpots();
            do {
                DGSpot* spot = currentNode->currentSpot();
                
                if (spot->hasTexture() && spot->isEnabled()) {
                    spot->texture()->bind();
                    _render->drawPolygon(spot->arrayOfCoordinates(), spot->face());
                }
            } while (currentNode->iterateSpots());
        }
    }
}

void DGScene::fadeIn() {
    
}

void DGScene::fadeOut() {
    
}

void DGScene::prepareSplash() {
    
}

bool DGScene::scanOverlays() {
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

bool DGScene::scanSpots() {
    // Check if the current node has spots to draw, and also if
    // we aren't dragging the view
    if (_canDrawSpots) {
        DGNode* currentNode = _currentRoom->currentNode();
        
        // Check if the current node is enabled
        if (currentNode->isEnabled()) {
            _render->disableTextures();
            
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
                            _render->enableTextures();
                            return true;
                        }
                    } while (currentNode->iterateSpots());
                }
            }
            
            _render->enableTextures();
        }
    }
    
    return false;
}

void DGScene::setRoom(DGRoom* room) {
    _currentRoom = room;
    
    // Determine if spots can be drawn
}
