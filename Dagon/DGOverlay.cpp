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
#include "DGImage.h"
#include "DGOverlay.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGOverlay::DGOverlay() {
    _isVisible = false;
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

bool DGOverlay::isVisible() {
    return _isVisible;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

DGButton* DGOverlay::currentButton() {
    return *_itButton;
}

DGImage* DGOverlay::currentImage() {
    return *_itImage;  
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

void DGOverlay::beginIteratingButtons() {
    _itButton = _arrayOfButtons.begin();
}

void DGOverlay::beginIteratingImages() {
    _itImage = _arrayOfImages.begin();
}

bool DGOverlay::iterateButtons() {
    _itButton++;
    
    if (_itButton == _arrayOfButtons.end())
        return false;
    else
        return true;
}

bool DGOverlay::iterateImages() {
    _itImage++;
    
    if (_itImage == _arrayOfImages.end())
        return false;
    else
        return true;
}

void DGOverlay::hide() {
    _isVisible = false;
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
}

void DGOverlay::show() {
    _isVisible = true;
}
