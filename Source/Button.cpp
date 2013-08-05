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

#include <cassert>

#include "Button.h"
#include "Config.h"
#include "DGFontManager.h"
#include "DGTexture.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Button::Button() :
  config(Config::instance()),
  fontManager(DGFontManager::instance())
{
  _font = fontManager.loadDefault();
  this->setFadeSpeed(DGFadeNormal);
  this->setType(DGObjectButton);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

Button::~Button() {
  if (_hasAction)
    delete _action;
  
  if (_hasOnHoverTexture)
    delete _onHoverTexture;
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool Button::hasAction() {
  return _hasAction;
}

bool Button::hasOnHoverTexture() {
  return _hasOnHoverTexture;
}

bool Button::hasFont() {
  return _hasFont;
}

bool Button::hasText() {
  return _hasText;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

Action Button::action() {
  assert(_hasAction == true);
  return *_action;
}

DGFont* Button::font() {
  return _font;
}

DGTexture* Button::onHoverTexture() {
  return _onHoverTexture;
}

std::string Button::text() {
  return _text;
}

int Button::textColor() {
  return _textColor;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void Button::setAction(Action anAction) {
  _action = new Action;
  *_action = anAction;
  _hasAction = true;
}

void Button::setFont(const std::string &fromFileName,
                       unsigned int heightOfFont) {
  // FIXME: Wrong, this can load many repeated fonts!
  _font = fontManager.load(fromFileName.c_str(), heightOfFont);
}

void Button::setOnHoverTexture(const std::string &fromFileName) {
  // TODO: Important! We should determine first if the texture already exists,
  // to avoid repeating resources. Eventually, this would be done via the
  // resource manager.
  _onHoverTexture = new DGTexture;
  _onHoverTexture->setResource(config.path(kPathResources, fromFileName,
                                   DGObjectImage).c_str());
  _onHoverTexture->load();
  _hasOnHoverTexture = true;
}

void Button::setText(std::string text){
  _text = text;
  _hasText = true;
}

void Button::setTextColor(int aColor) {
  // Note this expects one of our pre-generated colors
  _textColor = aColor;
}

void Button::updateCursor(int theCursor) {
  if (_hasAction) {
    _action->cursor = theCursor;
  }
}
