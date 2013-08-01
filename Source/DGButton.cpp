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

#include "DGButton.h"
#include "Config.h"
#include "DGFontManager.h"
#include "DGTexture.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGButton::DGButton() :
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

DGButton::~DGButton() {
  if (_hasAction)
    delete _actionData;
  
  if (_hasOnHoverTexture)
    delete _onHoverTexture;
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool DGButton::hasAction() {
  return _hasAction;
}

bool DGButton::hasOnHoverTexture() {
  return _hasOnHoverTexture;
}

bool DGButton::hasFont() {
  return _hasFont;
}

bool DGButton::hasText() {
  return _hasText;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

Action* DGButton::action() {
  return _actionData;
}

DGFont* DGButton::font() {
  return _font;
}

DGTexture* DGButton::onHoverTexture() {
  return _onHoverTexture;
}

std::string DGButton::text() {
  return _text;
}

int DGButton::textColor() {
  return _textColor;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void DGButton::setAction(Action* anAction) {
  _actionData = new Action;
  memcpy(_actionData, anAction, sizeof(Action));
  _hasAction = true;   
}

void DGButton::setFont(const std::string &fromFileName,
                       unsigned int heightOfFont) {
  // FIXME: Wrong, this can load many repeated fonts!
  _font = fontManager.load(fromFileName.c_str(), heightOfFont);
}

void DGButton::setOnHoverTexture(const std::string &fromFileName) {
  // TODO: Important! We should determine first if the texture already exists,
  // to avoid repeating resources. Eventually, this would be done via the
  // resource manager.
  DGTexture* texture;
    
  texture = new DGTexture;
  texture->setResource(config.path(kPathResources, fromFileName,
                                   DGObjectImage).c_str());
  texture->load();
  _onHoverTexture = texture;
  _hasOnHoverTexture = true;
}

void DGButton::setText(std::string text){
  _text = text;
  _hasText = true;
}

void DGButton::setTextColor(int aColor) {
  // Note this expects one of our pre-generated colors
  _textColor = aColor;
}
