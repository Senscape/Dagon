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

#include "Config.h"
#include "DGCursorManager.h"
#include "TextureManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGCursorManager::DGCursorManager() :
config(Config::instance())
{
  _hasAction = false;
  _hasImage = false;
  _isDragging = false;
  _onButton = false;
  _x = config.displayWidth / 2;
  _y = config.displayHeight / 2;
  
  this->setFadeSpeed(kFadeFastest); // Cursor always fades with fastest speed
  this->setSize(kDefCursorSize);
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

Action* DGCursorManager::action() {
  return _pointerToAction;
}

void DGCursorManager::bindImage() {
  (*_current).image->bind();
}

int* DGCursorManager::arrayOfCoords() {
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
void DGCursorManager::load(int typeOfCursor, const char* imageFromFile, int offsetX, int offsetY) {
  DGCursorData cursor;
  Texture* texture;
  
  texture = new Texture;
  texture->setResource(config.path(kPathResources, imageFromFile, kObjectCursor).c_str());
  texture->load();
  
  cursor.type = typeOfCursor;
  cursor.image = texture;
  cursor.origin.x = _half - offsetX;
  cursor.origin.y = _half - offsetY;
  
  _arrayOfCursors.push_back(cursor);
}

bool DGCursorManager::onButton() {
  return _onButton;
}

Point DGCursorManager::position() {
  Point pos;
  
  pos.x = _x;
  pos.y = _y;
  
  return pos;
}

void DGCursorManager::removeAction() {
  _pointerToAction = NULL;
  _hasAction = false;
}

void DGCursorManager::setDragging(bool flag) {
  if (flag) {
    this->_set(kCursorDrag);
  }
  
  _isDragging = flag;
}

void DGCursorManager::setOnButton(bool flag) {
  _onButton = flag;
}

void DGCursorManager::setAction(Action* theAction) {
  _pointerToAction = theAction;
  _hasAction = true;
  
  // Set the image, if available
  this->_set(theAction->cursor);
}

void DGCursorManager::setCursor(int typeOfCursor) {
  this->_set(typeOfCursor);
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

void DGCursorManager::_set(int typeOfCursor) {
  if (!_arrayOfCursors.empty()) {
    _current = _arrayOfCursors.begin();
    
    while (_current != _arrayOfCursors.end()) {
      if ((*_current).type == typeOfCursor) {
        _hasImage = true;
        return;
      }
      
      _current++;
    }
  }
  
  _hasImage = false;
}
