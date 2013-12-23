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

#ifndef DAGON_OVERLAY_H_
#define DAGON_OVERLAY_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <vector>

#include "Geometry.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class Button;
class Image;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class Overlay : public Object {
 public:
  Overlay();
  ~Overlay() {};
  
  // Checks
  bool hasButtons();
  bool hasImages();
  Point position();
  
  // Gets
  Button* currentButton();
  Image* currentImage();
  bool isLocked();
  
  // Sets
  void setPosition(int x, int y);
  
  // State changes
  Button* addButton(Button* aButton);
  Image* addImage(Image* anImage);
  void beginIteratingButtons(bool iterateBackwards);
  void beginIteratingImages();
  void fadeIn();
  void fadeOut();
  bool iterateButtons();
  bool iterateImages();
  void move(int offsetX, int offsetY); // Applies move to every element
  
 private:
  std::vector<Button*> _arrayOfButtons;
  std::vector<Image*> _arrayOfImages;
  std::vector<Button*>::iterator _itButton;
  std::vector<Button*>::reverse_iterator _ritButton;
  std::vector<Image*>::iterator _itImage;
  
  bool _isLocked;
  bool _isIteratingBackwards;
  Point _position;
  
  Overlay(const Overlay&);
  void operator=(const Overlay&);
};
  
}

#endif // DAGON_OVERLAY_H_
