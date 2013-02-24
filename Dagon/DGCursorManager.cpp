////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2012 Senscape s.r.l.
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

#include "DGConfig.h"
#include "DGCursorManager.h"
#include "DGTextureManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGCursorManager::DGCursorManager() {
    config = &DGConfig::getInstance();
    _hasAction = false;
    _hasImage = false;
    _isDragging = false;
    _onButton = false;
    _x = config->displayWidth / 2;
    _y = config->displayHeight / 2;
    
    this->setFadeSpeed(DGFadeFastest); // Cursor always fades with fastest speed
    this->setSize(DGDefCursorSize);
    this->updateCoords(_x, _y);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGCursorManager::~DGCursorManager() { 
    if (!_arrayOfCursors.empty()) {
        _current = _arrayOfCursors.begin();
        
        while (_current != _arrayOfCursors.end()) {
            delete (*_current).image;
            _current++;
        }   
    } 
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

DGAction* DGCursorManager::action() {
    return _pointerToAction;
}

void DGCursorManager::bindImage() {
    (*_current).image->bind();
}

float* DGCursorManager::arrayOfCoords() {
    return _arrayOfCoords;
}

bool DGCursorManager::hasAction() {
    return _hasAction;
}

bool DGCursorManager::hasImage() {
    return _hasImage;
}

bool DGCursorManager::isDragging() {
    return _isDragging;
}

// NOTE: These textures aren't managed
void DGCursorManager::load(int type, const char* imageFromFile, int offsetX, int offsetY) {
    DGCursorData cursor;
    DGTexture* texture;
    
    texture = new DGTexture;
    texture->setResource(config->path(DGPathRes, imageFromFile, DGObjectCursor));
    texture->load();
    
    cursor.type = type;
    cursor.image = texture;
    cursor.origin.x = _half - offsetX;
    cursor.origin.y = _half - offsetY;

    _arrayOfCursors.push_back(cursor);
}

bool DGCursorManager::onButton() {
    return _onButton;
}

DGPoint DGCursorManager::position() {
    DGPoint position;
    
    position.x = _x;
    position.y = _y;
    
    return position;
}

void DGCursorManager::removeAction() {
    _pointerToAction = NULL;
    _hasAction = false;
}

void DGCursorManager::setDragging(bool flag) {
    if (flag) {
        this->_set(DGCursorDrag);
    }
    
    _isDragging = flag;
}

void DGCursorManager::setOnButton(bool flag) {
    _onButton = flag;
}

void DGCursorManager::setAction(DGAction* action) {
    _pointerToAction = action;
    _hasAction = true;
    
    // Set the image, if available
    this->_set(action->cursor);
}

void DGCursorManager::setCursor(int type) {
    this->_set(type);
}

void DGCursorManager::setSize(int size) {
    _size = size;
    _half = _size / 2;
    
    this->updateCoords(_x, _y);
}

void DGCursorManager::updateCoords(int x, int y) {
    _x = x;
    _y = y;
    
    // NOTE: I have no idea why I commented this out. I'm sure there was
    // a good reason, so for now let's leave it like this, shall we?
    /*if (_hasImage) {
        _arrayOfCoords[0] = _x - _half + (*_current).origin.x;
        _arrayOfCoords[1] = _y - _half + (*_current).origin.y;
        _arrayOfCoords[2] = _x + _half + (*_current).origin.x;
        _arrayOfCoords[3] = _y - _half + (*_current).origin.y;
        _arrayOfCoords[4] = _x + _half + (*_current).origin.x;
        _arrayOfCoords[5] = _y + _half + (*_current).origin.y;
        _arrayOfCoords[6] = _x - _half + (*_current).origin.x;
        _arrayOfCoords[7] = _y + _half + (*_current).origin.y;
    }
    else {*/
        _arrayOfCoords[0] = _x - _half;
        _arrayOfCoords[1] = _y - _half;
        _arrayOfCoords[2] = _x + _half;
        _arrayOfCoords[3] = _y - _half;
        _arrayOfCoords[4] = _x + _half;
        _arrayOfCoords[5] = _y + _half;
        _arrayOfCoords[6] = _x - _half;
        _arrayOfCoords[7] = _y + _half;
   // }
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void DGCursorManager::_set(int type) {
    if (!_arrayOfCursors.empty()) {
        _current = _arrayOfCursors.begin();
        
        while (_current != _arrayOfCursors.end()) {
            if ((*_current).type == type) {
                _hasImage = true;
                return;
            }
            
            _current++;
        } 
    }
    
    _hasImage = false;
}
