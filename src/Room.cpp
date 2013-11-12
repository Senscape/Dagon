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
#include "Room.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Room::Room() {
  _currentNode = NULL;
  _hasDefaultFootstep = false;
  _hasEnterEvent = false;
  this->setType(kObjectRoom);
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool Room::hasAudios() {
  return !_arrayOfAudios.empty();
}

bool Room::hasDefaultFootstep() {
  return _hasDefaultFootstep;
}

bool Room::hasNodes() {
  // We invert the return value to comply with our method
  return !_arrayOfNodes.empty();
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

std::vector<Audio*> Room::arrayOfAudios() {
  return _arrayOfAudios;
}

Node* Room::currentNode() {
  assert(_currentNode != NULL);
  return _currentNode;
}

Audio* Room::defaultFootstep() {
  return _defaultFootstep;
}

int Room::effectsFlags() {
  return _effectsFlags;
}

Node* Room::iterator() {
  assert(_it != _arrayOfNodes.end());
  return *_it;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void Room::setDefaultFootstep(Audio* theFootstep) {
  _defaultFootstep = theFootstep;
  _hasDefaultFootstep = true;
}

void Room::setEffects(int theEffectFlags) {
  _effectsFlags = theEffectFlags;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

Audio* Room::addAudio(Audio* anAudio) {
  _arrayOfAudios.push_back(anAudio);
  return anAudio;
}

Node* Room::addNode(Node* aNode) {
  if (_arrayOfNodes.empty())
    _currentNode = aNode;
  aNode->setParentRoom(this);
  _arrayOfNodes.push_back(aNode);
  return aNode;
}

void Room::beginIteratingNodes() {
  _it = _arrayOfNodes.begin();
}
  
bool Room::hasEnterEvent() {
  return _hasEnterEvent;
}

bool Room::iterateNodes() {
  ++_it;
  if (_it == _arrayOfNodes.end()) {
    return false;
  } else {
    return true;
  }
}

int Room::enterEvent() {
  assert(_hasEnterEvent = true);
  return _luaReference;
}
  
void Room::setEnterEvent(int luaReference) {
  _hasEnterEvent = true;
  _luaReference = luaReference;
}

bool Room::switchTo(Node* theNode) {
  std::vector<Node*>::iterator it;
  for (it = _arrayOfNodes.begin(); it != _arrayOfNodes.end(); ++it) {
    if (*it == theNode) {
      // Node found, OK
      _currentNode = theNode;
      return true;
    }
  }
  return false;
}
  
}
