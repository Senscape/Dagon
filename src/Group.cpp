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

#include "Group.h"
#include "Spot.h"

namespace dagon {
  
////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////
  
Group::Group() {
  this->setType(kObjectGroup);
}
  
////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

std::string Group::description() {
  return _description;
}
  
////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void Group::setDescription(std::string theDescription) {
  _description = theDescription;
}
  
////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

Spot* Group::addSpot(Spot* aSpot) {
  _arrayOfSpots.push_back(aSpot);
  aSpot->setGroup(this);
  return aSpot;
}
  
void Group::disable() {
  std::vector<Spot*>::iterator _it = _arrayOfSpots.begin();
  while (_it != _arrayOfSpots.end()) {
    Spot* spot = (*_it);
    spot->disable(true);
    ++_it;
  }
}

void Group::enable() {
  std::vector<Spot*>::iterator _it = _arrayOfSpots.begin();
  while (_it != _arrayOfSpots.end()) {
    Spot* spot = (*_it);
    spot->enable(true);
    ++_it;
  }
}
  
void Group::play() {
  std::vector<Spot*>::iterator _it = _arrayOfSpots.begin();
  while (_it != _arrayOfSpots.end()) {
    Spot* spot = (*_it);
    spot->play();
    ++_it;
  }
}
  
void Group::stop() {
  std::vector<Spot*>::iterator _it = _arrayOfSpots.begin();
  while (_it != _arrayOfSpots.end()) {
    Spot* spot = (*_it);
    spot->stop();
    ++_it;
  }
}

}
