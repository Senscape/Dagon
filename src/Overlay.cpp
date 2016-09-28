////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
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

#include <cassert>

#include "Button.h"
#include "Defines.h"
#include "Image.h"
#include "Overlay.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Overlay::Overlay() {
  _position.x = 0;
  _position.y = 0;
  _isIteratingBackwards = false;
  _isLocked = false;
  _defaultCursor = kCursorNormal;
  this->disable(); // Overlays are disabled by default
  this->setType(kObjectOverlay);
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool Overlay::hasButtons() {
  return !_arrayOfButtons.empty();
}

bool Overlay::hasImages() {
  return !_arrayOfImages.empty();
}

Point Overlay::position() {
  return _position;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

Button* Overlay::currentButton() {
  if (_isIteratingBackwards) {
    assert(_ritButton != _arrayOfButtons.rend());
    return *_ritButton;
  } else {
    assert(_itButton != _arrayOfButtons.end());
    return *_itButton;
  }
}

Image* Overlay::currentImage() {
  assert(_itImage != _arrayOfImages.end());
  return *_itImage;
}
  
bool Overlay::isLocked() {
  return _isLocked;
}

int Overlay::defaultCursor() const {
  return _defaultCursor;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void Overlay::setPosition(int x, int y) {
  // Calculate the offsets for the new position
  int offsetX = x - _position.x;
  int offsetY = y - _position.y;
  
  // Automatically reposition all elements contained
  this->move(offsetX, offsetY);
  
  // Store the new position
  _position.x = x;
  _position.y = y;
}

void Overlay::setDefaultCursor(int cursor) {
  if (cursor >= kCursorNormal && kCursorCustom >= cursor) {
    _defaultCursor = cursor;
  }
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

Button* Overlay::addButton(Button* aButton) {
  _arrayOfButtons.push_back(aButton);
  return aButton;
}

Image* Overlay::addImage(Image* anImage) {
  _arrayOfImages.push_back(anImage);
  return anImage;
}

void Overlay::beginIteratingButtons(bool iterateBackwards) {
  if (iterateBackwards) {
    _ritButton = _arrayOfButtons.rbegin();
  } else {
    _itButton = _arrayOfButtons.begin();
  }
  _isIteratingBackwards = iterateBackwards;
}

void Overlay::beginIteratingImages() {
  _itImage = _arrayOfImages.begin();
}

void Overlay::fadeIn() {
  // Force enabling the overlay
  _isLocked = false;
  this->enable();
  
  // Buttons
  if (!_arrayOfButtons.empty()) {
    std::vector<Button*>::iterator itButton;
    itButton = _arrayOfButtons.begin();
    while (itButton != _arrayOfButtons.end()) {
      (*itButton)->fadeIn();
      ++itButton;
    }
  }
  
  // Images
  if (!_arrayOfImages.empty()) {
    std::vector<Image*>::iterator itImage;
    itImage = _arrayOfImages.begin();
    while (itImage != _arrayOfImages.end()) {
      (*itImage)->fadeIn();
      ++itImage;
    }
  }
}

void Overlay::fadeOut() {
  // Force disabling the overlay
  _isLocked = true;
  
  // Buttons
  if (!_arrayOfButtons.empty()) {
    std::vector<Button*>::iterator itButton;
    itButton = _arrayOfButtons.begin();
    while (itButton != _arrayOfButtons.end()) {
      (*itButton)->fadeOut();
      ++itButton;
    }
  }
  
  // Images
  if (!_arrayOfImages.empty()) {
    std::vector<Image*>::iterator itImage;
    itImage = _arrayOfImages.begin();
    while (itImage != _arrayOfImages.end()) {
      (*itImage)->fadeOut();
      ++itImage;
    }
  }
}

bool Overlay::iterateButtons() {
  if (_isIteratingBackwards) {
    ++_ritButton;
    if (_ritButton == _arrayOfButtons.rend()) {
      return false;
    } else {
      return true;
    }
  } else {
    ++_itButton;
    if (_itButton == _arrayOfButtons.end()) {
      return false;
    } else {
      return true;
    }
  }
}

bool Overlay::iterateImages() {
  ++_itImage;
  if (_itImage == _arrayOfImages.end()) {
    return false;
  } else {
    return true;
  }
}

void Overlay::move(int offsetX, int offsetY) {
  // Buttons
  if (!_arrayOfButtons.empty()) {
    std::vector<Button*>::iterator itButton;
    itButton = _arrayOfButtons.begin();
    while (itButton != _arrayOfButtons.end()) {
      (*itButton)->move(offsetX, offsetY);
      ++itButton;
    }
  }
  
  // Images
  if (!_arrayOfImages.empty()) {
    std::vector<Image*>::iterator itImage;
    itImage = _arrayOfImages.begin();
    while (itImage != _arrayOfImages.end()) {
      (*itImage)->move(offsetX, offsetY);
      ++itImage;
    }
  }
  
  // Store the new position
  _position.x += offsetX;
  _position.y += offsetY;
}
  
}
