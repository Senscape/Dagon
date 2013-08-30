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

#include "Audio.h"
#include "Spot.h"
#include "DGTexture.h"
#include "DGVideo.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Spot::Spot(std::vector<int> withArrayOfCoordinates,
               unsigned int onFace, int withFlags) {
  _arrayOfCoordinates = withArrayOfCoordinates;
  _onFace = onFace;
  _flags = withFlags;
  this->setType(kObjectSpot);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

Spot::~Spot() {
  if (_hasAction)
    delete _actionData;
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool Spot::hasAction() {
  return _hasAction;
}

bool Spot::hasAudio() {
  return _hasAudio;
}

bool Spot::hasColor() {
  return _hasColor;
}

bool Spot::hasFlag(int theFlag) {
  if (_flags & theFlag) {
    return true;
  } else {
    return false;
  }
}

bool Spot::hasTexture() {
  return _hasTexture;
}

bool Spot::hasVideo() {
  return _hasVideo;
}

bool Spot::isPlaying() {
  return _isPlaying;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

Action* Spot::action() {
  return _actionData;
}

Audio* Spot::audio() {
  return _attachedAudio;
}

int Spot::color() {
  return _color;
}

std::vector<int> Spot::arrayOfCoordinates() {
  return _arrayOfCoordinates;
}

unsigned int Spot::face() {
  return _onFace;
}

Point Spot::origin() {
  Point _origin;
  _origin.x = _arrayOfCoordinates[0];
  _origin.y = _arrayOfCoordinates[1];
  return _origin;
}

DGTexture* Spot::texture() {
  return _attachedTexture;
}

int Spot::vertexCount() {
  return (_arrayOfCoordinates.size() / 2.0);
}

DGVideo* Spot::video() {
  return _attachedVideo;
}

float Spot::volume() {
  return _volume;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void Spot::setAction(Action* anAction) {
  _actionData = new Action;
  _actionData->cursor = anAction->cursor;
  _actionData->feed = anAction->feed;
  _actionData->feedAudio = anAction->feedAudio;
  _actionData->hasFeedAudio = anAction->hasFeedAudio;
  _actionData->luaHandler = anAction->luaHandler;
  _actionData->luaObject = anAction->luaObject;
  _actionData->target = anAction->target;
  _actionData->type = anAction->type;
  _hasAction = true;
}

void Spot::setAudio(Audio* anAudio) {
  _attachedAudio = anAudio;
  _hasAudio = true;
}

void Spot::setColor(int aColor) {
  if (!aColor) {
    // Make sure we avoid black while generating the color
    int r, g, b;
    r = (rand() % 255) + 1;
    g = (rand() % 255) + 1;
    b = (rand() % 255) + 1;
    aColor = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
  }
  _color = aColor;
  _hasColor = true;
}

void Spot::setOrigin(int x, int y) {
  for (size_t i = 0; i < _arrayOfCoordinates.size(); i += 2) {
    _arrayOfCoordinates[i] += x;
    _arrayOfCoordinates[i + 1] += y;
  }
  _xOrigin = x;
  _yOrigin = y;
}

void Spot::setTexture(DGTexture* aTexture) {
  _attachedTexture = aTexture;
  _hasTexture = true;
}

void Spot::setVideo(DGVideo* aVideo) {
  _attachedVideo = aVideo;
  _hasVideo = true;
}

void Spot::setVolume(float theVolume) {
  _volume = theVolume;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void Spot::play() {
  _isPlaying = true;
    
  // FIXME: Should start playing only if in the current room
  if (_hasAudio && _attachedAudio->isLoaded())
    _attachedAudio->play();
    
  if (_hasVideo && _attachedVideo->isLoaded())
    _attachedVideo->play();
    
  // Hack of sorts but works OK
  if (this->hasFlag(kSpotLoop))
    _flags = _flags | kSpotAuto;
}

void Spot::resize(int width, int height) {
  Point origin;
  origin.x = _arrayOfCoordinates[0];
  origin.y = _arrayOfCoordinates[1];
  _arrayOfCoordinates.resize(8);
  _arrayOfCoordinates[0] = origin.x;
  _arrayOfCoordinates[1] = origin.y;
  _arrayOfCoordinates[2] = origin.x + width;
  _arrayOfCoordinates[3] = origin.y;
  _arrayOfCoordinates[4] = origin.x + width;
  _arrayOfCoordinates[5] = origin.y + height;
  _arrayOfCoordinates[6] = origin.x;
  _arrayOfCoordinates[7] = origin.y + height;
}

void Spot::stop() {
  _isPlaying = false;
  if (_hasAudio)
    _attachedAudio->stop();    
}
