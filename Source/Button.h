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

#ifndef DAGON_BUTTON_H_
#define DAGON_BUTTON_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <string>

#include "Action.h"
#include "Colors.h"
#include "Image.h"

////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////

class Config;
class Font;
class DGFontManager;
class Texture;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class Button : public Image {    
 public:
  Button();
  ~Button();

  // Checks
  bool hasAction();
  bool hasFont();
  bool hasOnHoverTexture();
  bool hasText();

  // Gets
  Action action();
  Font* font();
  Texture* onHoverTexture();
  std::string text();
  int textColor();

  // Sets
  void setAction(Action anAction);
  void setFont(const std::string &fromFileName, unsigned int heightOfFont);
  void setOnHoverTexture(const std::string &fromFileName);
  void setText(std::string text);
  void setTextColor(int aColor);
  void updateCursor(int theCursor);
  
 private:
  Config& config;
  DGFontManager& fontManager;
  
  Action* _action;
  Texture* _onHoverTexture;
  Font* _font;
  bool _hasAction = false;
  bool _hasFont = false;
  bool _hasOnHoverTexture = false;
  bool _hasText = false;
  std::string _text;
  int _textColor = kColorWhite;
  
  Button(const Button&);
  void operator=(const Button&);
};

#endif // DAGON_BUTTON_H_
