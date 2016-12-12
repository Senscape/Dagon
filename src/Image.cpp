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

#include <cstring>
#include <fstream>

namespace dagon {

static const unsigned char PNG_HEADER[8] = {137, 80, 78, 71, 13, 10, 26, 10};

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
  std::ifstream bitmapFile(_attachedTexture->resource(), std::ios::in |
                                                         std::ios::binary);
  if (!bitmapFile.good()) {
    _attachedTexture->loadBitmap();
  }
  else {
    char header[8];
    bitmapFile.read(header, 8);
    if (std::memcmp(header, PNG_HEADER, 8) == 0) {
      char buf[4]; // Width and height are stored as 32 bit words.
      bitmapFile.seekg(16); // Skip header (8 bytes) and IHDR chunk header (8 bytes).

      bitmapFile.read(buf, 4); // Read width (big endian integer).
      int width = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];

      bitmapFile.read(buf, 4); // Read height (big endian integer).
      int height = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];

      if (width <= 0 || height <= 0) { // In this case we should let STBI handle this file.
        _attachedTexture->loadBitmap();
      }
      else {
        _rect.origin = ZeroPoint;
        _rect.size = MakeSize(width, height);
        _calculateCoordinates();
      }
    }
    else {
      _attachedTexture->loadBitmap();
    }
  }

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
