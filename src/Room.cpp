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

#include <cassert>

#include "Audio.h"
#include "AudioManager.h"
#include "Node.h"
#include "Room.h"
#include "Spot.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Room::Room() {
  _currentNode = NULL;
  _hasDefaultFootstep = false;
  _hasEffects = false;
  _hasEnterEvent = false;
  _hasPersistEvent = false;
  _hasUnpersistEvent = false;
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
  
bool Room::hasEnterEvent() {
  return _hasEnterEvent;
}
  
bool Room::hasEffects() {
  return _hasEffects;
}

bool Room::hasNodes() {
  // We invert the return value to comply with our method
  return !_arrayOfNodes.empty();
}

bool Room::hasPersistEvent() {
  return _hasPersistEvent;
}

bool Room::hasUnpersistEvent() {
  return _hasUnpersistEvent;
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

SettingCollection Room::effects() {
  return _theEffects;
}
  
int Room::enterEvent() {
  assert(_hasEnterEvent = true);
  return _luaReference;
}

int Room::persistEvent() {
  assert(_hasPersistEvent == true);
  return _persistReference;
}

int Room::unpersistEvent() {
  assert(_hasUnpersistEvent == true);
  return _unpersistReference;
}

size_t Room::numNodes() {
  return _arrayOfNodes.size();
}

Node* Room::iterator() {
  assert(_it != _arrayOfNodes.end());
  return *_it;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void Room::setDefaultFootstep(Audio* theFootstep) {
  theFootstep->setVarying(true);
  _defaultFootstep = theFootstep;
  _hasDefaultFootstep = true;
}

void Room::setEffects(const SettingCollection& theEffects) {
  _theEffects = theEffects;
  _hasEffects = true;
}
  
void Room::setEnterEvent(int luaReference) {
  _hasEnterEvent = true;
  _luaReference = luaReference;
}

void Room::setPersistEvent(const int luaRef) {
  _hasPersistEvent = true;
  _persistReference = luaRef;
}

void Room::setUnpersistEvent(const int luaRef) {
  _hasUnpersistEvent = true;
  _unpersistReference = luaRef;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

Audio* Room::addAudio(Audio* anAudio) {
  _arrayOfAudios.push_back(anAudio);
  anAudio->setFadeSpeed(kFadeSlow);
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

bool Room::iterateNodes() {
  ++_it;
  if (_it == _arrayOfNodes.end()) {
    return false;
  } else {
    return true;
  }
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

void Room::claimAssets() {
  claimAudio();
}

void Room::claimAsset(Object* obj) {
  switch (obj->type()) {
  case kObjectInternalAudio:
  case kObjectAudio:
    Audio* audio = static_cast<Audio*>(obj);
    _claimedAssets.emplace(AudioManager::instance().asAsset(audio->filename()));
    break;
  }
}

void Room::releaseAssets() {
  for (auto audio : _arrayOfAudios) {
    if (!audio->isPlaying()) {
      _claimedAssets.erase(AudioManager::instance().asAsset(audio->filename()));
    }
    else if (!audio->isFading()) {
      audio->fadeOut();
    }
  }

  for (auto node : _arrayOfNodes) {
    if (node->hasSpots()) {
      node->beginIteratingSpots();

      do {
        Spot* spot = node->currentSpot();

        if (spot->hasAudio()) {
          if (spot->audio()->isPlaying()) {
            spot->audio()->stop();
          }
          
          _claimedAssets.erase(AudioManager::instance().asAsset(spot->audio()->filename()));
        }
      } while (node->iterateSpots());
    }
  }
}

void Room::claimAudio() {
  for (const auto audio : _arrayOfAudios) {
    claimAsset(audio);
  }

  for (auto node : _arrayOfNodes) {
    if (node->hasSpots()) {
      node->beginIteratingSpots();

      do {
        Spot* spot = node->currentSpot();

        if (spot->hasAudio()) {
          claimAsset(spot->audio());
        }
      } while (node->iterateSpots());
    }
  }
}
  
}
