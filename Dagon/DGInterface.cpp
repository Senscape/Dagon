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
#include "DGCameraManager.h"
#include "DGConfig.h"
#include "DGCursorManager.h"
#include "DGFont.h"
#include "DGImage.h"
#include "DGInterface.h"
#include "DGOverlay.h"
#include "DGRenderManager.h"
#include "DGTexture.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGInterface::DGInterface() {
    cameraManager = &DGCameraManager::getInstance(); 
    config = &DGConfig::getInstance();  
    cursorManager = &DGCursorManager::getInstance();
    renderManager = &DGRenderManager::getInstance();
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGInterface::~DGInterface() {
    // Nothing to do here
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGInterface::addOverlay(DGOverlay* overlay) {
    _arrayOfOverlays.push_back(overlay);
}

void DGInterface::drawCursor() {
    renderManager->setAlpha(1.0f);
    
    // Mouse cursor
    if (cursorManager->isEnabled()) {
        if (cursorManager->hasImage()) { // A bitmap cursor is currently set
            cursorManager->update(); // Process fade (supported only with bitmaps)
            cursorManager->bindImage();
            renderManager->setAlpha(cursorManager->fadeLevel());
            renderManager->drawSlide(cursorManager->arrayOfCoords());
        }
        else {
            DGPoint position = cursorManager->position();
            
            renderManager->disableTextures(); // Default cursor doesn't require textures
            if (cursorManager->onButton() || cursorManager->hasAction())
                renderManager->setColor(DGColorBrightRed);
            else
                renderManager->setColor(DGColorDarkGray);
            renderManager->drawHelper(position.x, position.y, false);
            renderManager->enableTextures();
        }
    }
}

void DGInterface::drawHelpers() {
    renderManager->disableTextures();
    
    // Helpers
    if (config->showHelpers) {
        if (renderManager->beginIteratingHelpers()) { // Check if we have any
            do {
                DGPoint point = renderManager->currentHelper();
                renderManager->setColor(DGColorBrightCyan);
                renderManager->drawHelper(point.x, point.y, true);
                
            } while (renderManager->iterateHelpers());
        }
    }
    
    renderManager->enableTextures();
}

void DGInterface::drawOverlays() {
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
                                renderManager->setAlpha(button->fadeLevel());
                                button->texture()->bind();
                                renderManager->drawSlide(button->arrayOfCoordinates());
                            }
                            
                            if (button->hasText()) {
                                DGPoint position = button->position();
                                renderManager->setColor(button->textColor());
                                button->font()->print(position.x, position.y, button->text());
                                renderManager->setColor(DGColorWhite); // Reset the color
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
                            renderManager->setAlpha(image->fadeLevel());
                            renderManager->drawSlide(image->arrayOfCoordinates());
                        }
                    } while ((*itOverlay)->iterateImages());
                }
                
            }
            
            itOverlay++;
        }
    }   
}

bool DGInterface::scanOverlays() {
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
