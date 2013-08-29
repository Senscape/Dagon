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

#ifndef DAGON_NODE_H_
#define DAGON_NODE_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <string>
#include <vector>

#include "Action.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class Audio;
class DGSpot;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class Node : public Object {
 public:
  Node();
  ~Node();
  
  // Checks
  bool hasBundleName();
  bool hasFootstep();
  bool hasSpots();
  bool isSlide();
  
  // Gets
  std::string bundleName();
  DGSpot* currentSpot();  
  std::string description();
  Audio* footstep();
  Node* previousNode();
  int slideReturn();
  
  // Sets
  void setBundleName(std::string name);
  void setDescription(std::string description);
  void setFootstep(Audio* theFootstep);
  void setPreviousNode(Node* node);
  void setSlide(bool enabled);
  void setSlideReturn(int luaHandler);
  
  // State changes
  void addCustomLink(unsigned int withDirection, int luaHandler);
  void addLink(unsigned int withDirection, Object* theTarget);
  DGSpot* addSpot(DGSpot* aSpot);
  void beginIteratingSpots();
  bool iterateSpots();
  
 private:
  // The bundle name is neither a filename or name of the node,
  // the Texture Manager uses it later to generate the corresponding
  // filenames. This would be the name of the Lua object.
  std::string _bundleName;
  std::vector<DGSpot*> _arrayOfSpots;
  std::vector<DGSpot*>::iterator _it;
  std::string _description;
  
  Audio* _footstep;
  Node* _previousNode;
  bool _hasFootstep = false;
  bool _isSlide = false;
  int _slideReturn = 0;
    
  void _link(unsigned int direction, Action* action);
  
  Node(const Node&);
  void operator=(const Node&);
};

#endif // DAGON_NODE_H_
