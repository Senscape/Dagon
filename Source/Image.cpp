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
#include "Image.h"
#include "Texture.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Image::Image() :
config(Config::instance())
{
  _hasTexture = false;
  _rect.origin.x = 0;
  _rect.origin.y = 0;
  _rect.size.width = 0;
  _rect.size.height = 0;
  this->setType(kObjectImage);
}

Image::Image(const std::string &fromFileName) :
config(Config::instance())
{
  this->setTexture(fromFileName);
  if (_attachedTexture->isLoaded()) {
    _rect.origin.x = 0;
    _rect.origin.y = 0;
    _rect.size.width = _attachedTexture->width();
    _rect.size.height = _attachedTexture->height();
    _calculateCoordinates();
  }
  this->setType(kObjectImage);
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool Image::hasTexture() {
  return _hasTexture;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

int* Image::arrayOfCoordinates() {
  return _arrayOfCoordinates;
}

Point Image::position() {
  return _rect.origin;
}

Size Image::size() {
  return _rect.size;
}

Texture* Image::texture() {
  return _attachedTexture;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void Image::setPosition(int x, int y) {
  _rect.origin.x = x;
  _rect.origin.y = y;
  _calculateCoordinates();
}

void Image::setSize(int width, int height) {
  _rect.size.width = width;
  _rect.size.height = height;
  _calculateCoordinates();
}

void Image::setTexture(const std::string &fromFileName) {
  // TODO: Important! We should determine first if the texture already exists
  // to avoid repeating resources. Eventually, this would be done via the
  // resource manager.
  // FIXME: These textures are immediately loaded which isn't very efficient.
  
  _attachedTexture = new Texture;
  _attachedTexture->setResource(config.path(kPathResources, fromFileName,
                                            kObjectImage).c_str());
  _attachedTexture->load();
  _hasTexture = true;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void Image::move(int offsetX, int offsetY) {
  _rect.origin.x += offsetX;
  _rect.origin.y += offsetY;
  _calculateCoordinates();
}

void Image::scale(float factor) {
  _rect.size.width *= factor;
  _rect.size.height *= factor;
  _calculateCoordinates();
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void Image::_calculateCoordinates() {
  _arrayOfCoordinates[0] = _rect.origin.x;
  _arrayOfCoordinates[1] = _rect.origin.y;
  _arrayOfCoordinates[2] = _rect.origin.x + _rect.size.width;
  _arrayOfCoordinates[3] = _rect.origin.y;
  _arrayOfCoordinates[4] = _rect.origin.x + _rect.size.width;
  _arrayOfCoordinates[5] = _rect.origin.y + _rect.size.height;
  _arrayOfCoordinates[6] = _rect.origin.x;
  _arrayOfCoordinates[7] = _rect.origin.y + _rect.size.height;
}
