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

#ifndef DAGON_GROUP_H_
#define DAGON_GROUP_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <string>
#include <vector>

#include "Object.h"

namespace dagon {
  
////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class Spot;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class Group : public Object {
 public:
  Group();
  ~Group() {};
  
  // Gets
  std::string description();
  
  // Sets
  void setDescription(std::string theDescription);
  
  // State changes
  Spot* addSpot(Spot* aSpot);
  void disable();
  void enable();
  
 private:
  std::vector<Spot*> _arrayOfSpots;
  std::string _description;
  
  Group(const Group&);
  void operator=(const Group&);
};
  
}

#endif // DAGON_GROUP_H_
