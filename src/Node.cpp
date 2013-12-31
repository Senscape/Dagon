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

#include "Audio.h"
#include "Node.h"
#include "Spot.h"
#include "Texture.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Node::Node() {
  _previousNode = this;
  _hasFootstep = false;
  _hasEnterEvent = false;
  _isSlide = false;
  _parentRoom = 0;
  _slideReturn = 0;
  this->setType(kObjectNode);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

Node::~Node() {
  // Note that we only delete spots automatically created by this class,
  // not ones by the user
  _it = _arrayOfSpots.begin();
  while (_it != _arrayOfSpots.end()) {
    Spot* spot = (*_it);
    if (spot->hasFlag(kSpotClass))
      delete spot;
    ++_it;
  }
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool Node::hasBundleName() {
  return !_bundleName.empty();
}

bool Node::hasFootstep() {
  return _hasFootstep;
}
  
bool Node::hasEnterEvent() {
  return _hasEnterEvent;
}
  
bool Node::hasLeaveEvent() {
  return _hasLeaveEvent;
}

bool Node::hasSpots() {
  return !_arrayOfSpots.empty();
}

bool Node::isSlide() {
  return _isSlide;
}

int Node::slideReturn() {
  return _slideReturn;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

std::string Node::bundleName() {
  return _bundleName;
}

Spot* Node::currentSpot() {
  assert(_it != _arrayOfSpots.end());
  return *_it;
}

std::string Node::description() {
  return _description;
}

int Node::enterEvent() {
  assert(_hasEnterEvent = true);
  return _luaEnterReference;
}
  
Audio* Node::footstep() {
  return _footstep;
}
  
int Node::leaveEvent() {
  assert(_hasLeaveEvent = true);
  return _luaLeaveReference;
}

Room* Node::parentRoom() {
  return _parentRoom;
}
  
Node* Node::previousNode() {
  return _previousNode;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void Node::setBundleName(std::string theName) {
  _bundleName = theName;
}

void Node::setDescription(std::string theDescription) {
  _description = theDescription;
}
  
void Node::setEnterEvent(int theLuaReference) {
  _hasEnterEvent = true;
  _luaEnterReference = theLuaReference;
}

void Node::setFootstep(Audio* theFootstep) {
  _footstep = theFootstep;
  _hasFootstep = true;
}
  
void Node::setLeaveEvent(int theLuaReference) {
  _hasLeaveEvent = true;
  _luaLeaveReference = theLuaReference;
}
  
void Node::setParentRoom(Room* room) {
  _parentRoom = room;
}

void Node::setPreviousNode(Node* node) {
  _previousNode = node;
}

void Node::setSlide(bool enabled) {
  _isSlide = enabled;
}

void Node::setSlideReturn(int luaHandler) {
  _slideReturn = luaHandler;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void Node::addCustomLink(unsigned withDirection, int luaHandler) {
  Action action;
  action.type = kActionFunction;
  action.cursor = kCursorForward;
  action.luaHandler = luaHandler;
  _link(withDirection, &action);
}

void Node::addLink(unsigned int withDirection, Object* theTarget) {
  Action action;
  action.type = kActionSwitch;
  action.cursor = kCursorForward;
  action.target = theTarget;
  _link(withDirection, &action);
}

Spot* Node::addSpot(Spot* aSpot) {
  _arrayOfSpots.push_back(aSpot);
  return aSpot;
}

void Node::beginIteratingSpots() {
  _it = _arrayOfSpots.begin();
}

bool Node::iterateSpots() {
  ++_it;
  if (_it == _arrayOfSpots.end()) {
    return false;
  } else {
    return true;
  }
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void Node::_link(unsigned int direction, Action* action) {
  // We ensure the texture is properly stretched, so we take the default
  // cube size.
  // TODO: This setting should be obtained directly from the Config class
  int minorBound = kDefTexSize / 3;
  int majorBound = static_cast<int>(kDefTexSize / 1.5f);
  int offset = kDefTexSize / 3;
  
  // We always have four corners here, hence eight elements since they are
  // squared spots.
  
  std::vector<int> arrayOfCoordinates;
  int coordsNormal[] = {minorBound, minorBound, majorBound, minorBound,
    majorBound, majorBound, minorBound, majorBound};
  int coordsShiftRight[] = {minorBound + offset, minorBound,
    majorBound + offset, minorBound, majorBound + offset, majorBound,
    minorBound + offset, majorBound};
  int coordsShiftLeft[] = {minorBound - offset, minorBound, majorBound - offset,
    minorBound, majorBound - offset, majorBound, minorBound - offset,
    majorBound};
  
  Spot *newSpot = NULL, *auxSpot = NULL;
  
  switch (direction) {
    case kNorth:
    case kEast:
    case kSouth:
    case kWest: {
      arrayOfCoordinates.assign(coordsNormal, coordsNormal + 8);
      newSpot = new Spot(arrayOfCoordinates, direction, kSpotClass);
      break;
    }
    case kNorthEast: {
      arrayOfCoordinates.assign(coordsShiftRight, coordsShiftRight + 8);
      newSpot = new Spot(arrayOfCoordinates, kNorth, kSpotClass);
      arrayOfCoordinates.assign(coordsShiftLeft, coordsShiftLeft + 8);
      auxSpot = new Spot(arrayOfCoordinates, kEast, kSpotClass);
      break;
    }
    case kSouthEast: {
      arrayOfCoordinates.assign(coordsShiftRight, coordsShiftRight + 8);
      newSpot = new Spot(arrayOfCoordinates, kEast, kSpotClass);
      arrayOfCoordinates.assign(coordsShiftLeft, coordsShiftLeft + 8);
      auxSpot = new Spot(arrayOfCoordinates, kSouth, kSpotClass);
      break;
    }
    case kSouthWest: {
      arrayOfCoordinates.assign(coordsShiftRight, coordsShiftRight + 8);
      newSpot = new Spot(arrayOfCoordinates, kSouth, kSpotClass);
      arrayOfCoordinates.assign(coordsShiftLeft, coordsShiftLeft + 8);
      auxSpot = new Spot(arrayOfCoordinates, kWest, kSpotClass);
      break;
    }
    case kNorthWest: {
      arrayOfCoordinates.assign(coordsShiftRight, coordsShiftRight + 8);
      newSpot = new Spot(arrayOfCoordinates, kWest, kSpotClass);
      arrayOfCoordinates.assign(coordsShiftLeft, coordsShiftLeft + 8);
      auxSpot = new Spot(arrayOfCoordinates, kNorth, kSpotClass);
      break;
    }
    default: {
      assert(false);
    }
  }
  
  newSpot->setAction(action);
  newSpot->setColor(0); // Color is set automatically
  _arrayOfSpots.push_back(newSpot);
  
  if (auxSpot) {
    auxSpot->setAction(action);
    auxSpot->setColor(0);
    _arrayOfSpots.push_back(auxSpot);
  }
}
  
}
