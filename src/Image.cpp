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

#include "Config.h"
#include "Image.h"
#include "Texture.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Image::Image() :
config(Config::instance())
{
  _hasTexture = false;
  _rect = ZeroRect;
  this->setType(kObjectImage);
}

Image::Image(const std::string &fromFileName) :
config(Config::instance())
{
  this->setTexture(fromFileName);
  if (_attachedTexture->isBitmapLoaded()) {
    _rect.origin = ZeroPoint;
    _rect.size = MakeSize(_attachedTexture->width(),
                          _attachedTexture->height());
    _calculateCoordinates();
    _attachedTexture->unloadBitmap();
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

float* Image::arrayOfCoordinates() {
  return _arrayOfCoordinates;
}

Point Image::position() {
  return _rect.origin;
}

Size Image::size() {
  return _rect.size;
}

Texture* Image::texture() {
  _attachedTexture->load();
  return _attachedTexture;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void Image::setPosition(float x, float y) {
  _rect.origin = MakePoint(x, y);
  _calculateCoordinates();
}

void Image::setSize(float width, float height) {
  _rect.size = MakeSize(width, height);
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
  _attachedTexture->loadBitmap();
  _hasTexture = true;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void Image::move(float offsetX, float offsetY) {
  MoveRect(_rect, offsetX, offsetY);
  _calculateCoordinates();
}

void Image::scale(float factor) {
  ScaleRect(_rect, factor);
  _calculateCoordinates();
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void Image::_calculateCoordinates() {
  _arrayOfCoordinates[0] = MinX(_rect);
  _arrayOfCoordinates[1] = MinY(_rect);
  _arrayOfCoordinates[2] = MaxX(_rect);
  _arrayOfCoordinates[3] = MinY(_rect);
  _arrayOfCoordinates[4] = MaxX(_rect);
  _arrayOfCoordinates[5] = MaxY(_rect);
  _arrayOfCoordinates[6] = MinX(_rect);
  _arrayOfCoordinates[7] = MaxY(_rect);
}

}
