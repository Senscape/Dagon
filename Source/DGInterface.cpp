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

#include "Button.h"
#include "DGCameraManager.h"
#include "Config.h"
#include "DGCursorManager.h"
#include "Font.h"
#include "Image.h"
#include "DGInterface.h"
#include "Overlay.h"
#include "DGRenderManager.h"
#include "Texture.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGInterface::DGInterface() :
cameraManager(DGCameraManager::instance()),
config(Config::instance()),
cursorManager(DGCursorManager::instance()),
renderManager(DGRenderManager::instance())
{
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

void DGInterface::addOverlay(Overlay* overlay) {
  _arrayOfOverlays.push_back(overlay);
}

void DGInterface::drawCursor() {
  renderManager.setAlpha(1.0f);
  
  // Mouse cursor
  if (cursorManager.isEnabled()) {
    if (cursorManager.hasImage()) { // A bitmap cursor is currently set
      cursorManager.updateFade(); // Process fade (supported only with bitmaps)
      cursorManager.bindImage();
      renderManager.setAlpha(cursorManager.fadeLevel());
      renderManager.drawSlide(cursorManager.arrayOfCoords());
    }
    else {
      Point position = cursorManager.position();
      
      renderManager.disableTextures(); // Default cursor doesn't require textures
      if (cursorManager.onButton() || cursorManager.hasAction())
        renderManager.setColor(kColorBrightRed);
      else
        renderManager.setColor(kColorDarkGray);
      renderManager.drawHelper(position.x, position.y, false);
      renderManager.enableTextures();
    }
  }
}

void DGInterface::drawHelpers() {
  renderManager.disableTextures();
  
  // Helpers
  if (config.showHelpers) {
    if (renderManager.beginIteratingHelpers()) { // Check if we have any
      do {
        Point point = renderManager.currentHelper();
        renderManager.setColor(kColorBrightCyan);
        renderManager.drawHelper(point.x, point.y, true);
        
      } while (renderManager.iterateHelpers());
    }
  }
  
  renderManager.enableTextures();
}

void DGInterface::drawOverlays() {
  if (!_arrayOfOverlays.empty()) {
    vector<Overlay*>::iterator itOverlay;
    
    itOverlay = _arrayOfOverlays.begin();
    
    while (itOverlay != _arrayOfOverlays.end()) {
      if ((*itOverlay)->isEnabled()) {
        
        // Draw buttons first
        if ((*itOverlay)->hasButtons()) {
          (*itOverlay)->beginIteratingButtons(false);
          
          do {
            Button* button = (*itOverlay)->currentButton();
            if (button->isEnabled()) {
              button->updateFade();
              
              if (button->hasTexture()) {
                renderManager.setAlpha(button->fadeLevel());
                button->texture()->bind();
                renderManager.drawSlide(button->arrayOfCoordinates());
              }
              
              if (button->hasText()) {
                Point position = button->position();
                // int color = button->textColor();
                if (button->isFading())
                  renderManager.setColor(button->textColor(), button->fadeLevel());
                else
                  renderManager.setColor(button->textColor());
                button->font()->print(position.x, position.y, button->text().c_str());
                renderManager.setColor(kColorWhite); // Reset the color
              }
            }
          } while ((*itOverlay)->iterateButtons());
        }
        
        // Now images
        // NOTE: This isn't quite right, images should be drawn first. Perhaps
        // allow the user to setup the drawing order for each overlay.
        if ((*itOverlay)->hasImages()) {
          (*itOverlay)->beginIteratingImages();
          
          do {
            Image* image = (*itOverlay)->currentImage();
            if (image->isEnabled()) {
              image->updateFade(); // Perform any necessary updates
              image->texture()->bind();
              renderManager.setAlpha(image->fadeLevel());
              renderManager.drawSlide(image->arrayOfCoordinates());
            }
          } while ((*itOverlay)->iterateImages());
        }
        
      }
      
      itOverlay++;
    }
  }
}

bool DGInterface::scanOverlays() {
  cursorManager.setOnButton(false);
  
  if (!_arrayOfOverlays.empty()) {
    vector<Overlay*>::reverse_iterator itOverlay;
    
    itOverlay = _arrayOfOverlays.rbegin();
    
    while (itOverlay != _arrayOfOverlays.rend()) {
      if ((*itOverlay)->isEnabled()) {
        
        if ((*itOverlay)->hasButtons()) {
          (*itOverlay)->beginIteratingButtons(true);
          
          do {
            Button* button = (*itOverlay)->currentButton();
            if (button->isEnabled()) {
              Point position = cursorManager.position();
              int* arrayOfCoordinates = button->arrayOfCoordinates();
              if (position.x >= arrayOfCoordinates[0] && position.y >= arrayOfCoordinates[1] &&
                  position.x <= arrayOfCoordinates[4] && position.y <= arrayOfCoordinates[5]) {
                
                cursorManager.setOnButton(true);
                if (button->hasAction()) {
                  Action action = button->action();
                  cursorManager.setAction(&action);
                }
                
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
