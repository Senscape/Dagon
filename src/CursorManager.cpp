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
#include "CursorManager.h"
#include "TextureManager.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

CursorManager::CursorManager() :
config(Config::instance())
{
  _hasAction = false;
  _hasImage = false;
  _isDragging = false;
  _onButton = false;
  _x = config.displayWidth >> 1;
  _y = config.displayHeight >> 1;
  
  this->setFadeSpeed(kFadeFastest); // Cursor always fades with fastest speed
  this->setSize(kDefCursorSize);
  this->updateCoords(_x, _y);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

CursorManager::~CursorManager() {
  if (!_arrayOfCursors.empty()) {
    _current = _arrayOfCursors.begin();
    
    while (_current != _arrayOfCursors.end()) {
      delete (*_current).image;
      ++_current;
    }
  }
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

Action* CursorManager::action() {
  return _pointerToAction;
}

void CursorManager::bindImage() {
  (*_current).image->bind();
}

float* CursorManager::arrayOfCoords() {
  return _arrayOfCoords;
}

bool CursorManager::hasAction() {
  return _hasAction;
}

bool CursorManager::hasImage() {
  return _hasImage;
}

bool CursorManager::isDragging() {
  return _isDragging;
}

// NOTE: These textures aren't managed
void CursorManager::load(int typeOfCursor, const char* imageFromFile, int offsetX, int offsetY) {
  DGCursorData cursor;
  Texture* texture;
  
  texture = new Texture;
  texture->setResource(config.path(kPathResources, imageFromFile, kObjectCursor).c_str());
  texture->load();
  
  cursor.type = typeOfCursor;
  cursor.image = texture;
  cursor.origin = MakePoint(_half - offsetX, _half - offsetY);
  
  _arrayOfCursors.push_back(cursor);
}

bool CursorManager::onButton() {
  return _onButton;
}

Point CursorManager::position() {
  Point pos = MakePoint(_x, _y);
  return pos;
}

void CursorManager::removeAction() {
  _pointerToAction = NULL;
  _hasAction = false;
}

void CursorManager::setDragging(bool flag) {
  if (flag) {
    this->_set(kCursorDrag);
  }
  
  _isDragging = flag;
}

void CursorManager::setOnButton(bool flag) {
  _onButton = flag;
}

void CursorManager::setAction(Action* theAction) {
  _pointerToAction = theAction;
  _hasAction = true;
  
  // Set the image, if available
  this->_set(theAction->cursor);
}

void CursorManager::setCursor(int typeOfCursor) {
  this->_set(typeOfCursor);
}

void CursorManager::setSize(int size) {
  _size = size;
  _half = _size >> 1;
  this->updateCoords(_x, _y);
}

void CursorManager::updateCoords(int x, int y) {
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

void CursorManager::_set(int typeOfCursor) {
  if (!_arrayOfCursors.empty()) {
    _current = _arrayOfCursors.begin();
    
    while (_current != _arrayOfCursors.end()) {
      if ((*_current).type == typeOfCursor) {
        _hasImage = true;
        return;
      }
      
      ++_current;
    }
  }
  
  _hasImage = false;
}
  
}
