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

#ifndef DAGON_IMAGE_H_
#define DAGON_IMAGE_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <string>

#include "Geometry.h"
#include "Object.h"

////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////

class Config;
class Texture;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class Image : public Object {
 public:
  Image();
  Image(const std::string &fromFileName);
  ~Image() {};
  
  // Checks
  bool hasTexture();
  
  // Gets
  int* arrayOfCoordinates();
  Point position();
  Size size();
  Texture* texture();
  
  // Sets
  void setPosition(int x, int y);
  void setSize(int width, int height);
  void setTexture(const std::string &fromFileName);
  
  // State changes
  void move(int offsetX, int offsetY);
  void scale(float factor);
  
 private:
  Config& config;
  
  int _arrayOfCoordinates[8];
  Texture* _attachedTexture;
  bool _hasTexture;
  Rect _rect;
  
  void _calculateCoordinates();
  
  Image(const Image&);
  void operator=(const Image&);
};

#endif // DAGON_IMAGE_H_
