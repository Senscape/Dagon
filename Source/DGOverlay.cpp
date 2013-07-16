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

#include "DGButton.h"
#include "DGImage.h"
#include "DGOverlay.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGOverlay::DGOverlay() {
    _isIteratingBackwards = false;
    _position.x = 0;
    _position.y = 0;
    
    this->disable(); // Overlays are disabled by default
    this->setType(DGObjectOverlay);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGOverlay::~DGOverlay() {
    // Nothing to do here
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool DGOverlay::hasButtons() {
    return !_arrayOfButtons.empty();
}

bool DGOverlay::hasImages() {
    return !_arrayOfImages.empty();
}

DGPoint DGOverlay::position() {
    return _position;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

DGButton* DGOverlay::currentButton() {
    if (_isIteratingBackwards)
        return *_ritButton;
    
    return *_itButton;
}

DGImage* DGOverlay::currentImage() {
    return *_itImage;  
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void DGOverlay::setPosition(int x, int y) {
    // Calculate the offsets for the new position
    int offsetX = x - _position.x;
    int offsetY = y - _position.y;
    
    // Automatically reposition all elements contained
    this->move(offsetX, offsetY);
    
    // Store the new position
    _position.x = x;
    _position.y = y;    
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

DGButton* DGOverlay::addButton(DGButton* aButton) {
    _arrayOfButtons.push_back(aButton);
    return aButton;
}

DGImage* DGOverlay::addImage(DGImage* anImage) {
    _arrayOfImages.push_back(anImage);
    return anImage;
}

void DGOverlay::beginIteratingButtons(bool iterateBackwards) {
    if (iterateBackwards)
        _ritButton = _arrayOfButtons.rbegin();
    else
        _itButton = _arrayOfButtons.begin();
    
    _isIteratingBackwards = iterateBackwards;
}

void DGOverlay::beginIteratingImages() {
    _itImage = _arrayOfImages.begin();
}

void DGOverlay::fadeIn() {
    this->enable();
    
    // Buttons
    if (!_arrayOfButtons.empty()) {
        vector<DGButton*>::iterator itButton;
        
        itButton = _arrayOfButtons.begin();
        
        while (itButton != _arrayOfButtons.end()) {
            (*itButton)->fadeIn();
            itButton++;
        }
    }
    
    // Images
    if (!_arrayOfImages.empty()) {
        vector<DGImage*>::iterator itImage;
        
        itImage = _arrayOfImages.begin();
        
        while (itImage != _arrayOfImages.end()) {
            (*itImage)->fadeIn();
            itImage++;
        }
    }
}

void DGOverlay::fadeOut() {
    // Buttons
    if (!_arrayOfButtons.empty()) {
        vector<DGButton*>::iterator itButton;
        
        itButton = _arrayOfButtons.begin();
        
        while (itButton != _arrayOfButtons.end()) {
            (*itButton)->fadeOut();
            itButton++;
        }
    }
    
    // Images
    if (!_arrayOfImages.empty()) {
        vector<DGImage*>::iterator itImage;
        
        itImage = _arrayOfImages.begin();
        
        while (itImage != _arrayOfImages.end()) {
            (*itImage)->fadeOut();
            itImage++;
        }
    }
}

bool DGOverlay::iterateButtons() {
    if (_isIteratingBackwards) {
        _ritButton++;
        if (_ritButton == _arrayOfButtons.rend()) return false;
        else return true;  
    }
    else {
        _itButton++;
        if (_itButton == _arrayOfButtons.end()) return false;
        else return true;
    }
    
    return false;
}

bool DGOverlay::iterateImages() {
    _itImage++;
    
    if (_itImage == _arrayOfImages.end())
        return false;
    else
        return true;
}

void DGOverlay::move(int offsetX, int offsetY) {
    // Buttons
    if (!_arrayOfButtons.empty()) {
        vector<DGButton*>::iterator itButton;
        
        itButton = _arrayOfButtons.begin();
        
        while (itButton != _arrayOfButtons.end()) {
            (*itButton)->move(offsetX, offsetY);
            itButton++;
        }
    }
    
    // Images
    if (!_arrayOfImages.empty()) {
        vector<DGImage*>::iterator itImage;
        
        itImage = _arrayOfImages.begin();
        
        while (itImage != _arrayOfImages.end()) {
            (*itImage)->move(offsetX, offsetY);
            itImage++;
        }
    }
    
    // Store the new position
    _position.x += offsetX;
    _position.y += offsetY;
}
