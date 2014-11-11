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

#ifndef DAGON_ROOM_H_
#define DAGON_ROOM_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <vector>
#include "Configurable.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class Audio;
class Node;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class Room : public Object {
 public:
  Room();
  ~Room() {};
  
  // Checks
  bool hasAudios();
  bool hasDefaultFootstep();
  bool hasEffects();
  bool hasNodes();
  
  // Gets
  std::vector<Audio*> arrayOfAudios();
  Node* currentNode();
  Audio* defaultFootstep();
  SettingCollection effects();
  int enterEvent();
  
  // Sets
  void setDefaultFootstep(Audio* theFootstep);
  void setEffects(const SettingCollection& theEffects);
  
  // State changes
  Audio* addAudio(Audio* anAudio);
  Node* addNode(Node* aNode);
  void beginIteratingNodes();
  bool hasEnterEvent();
  bool iterateNodes();
  Node* iterator();
  void setEnterEvent(int luaReference);
  bool switchTo(Node* theNode);
  
 private:
  SettingCollection _theEffects;
  
  std::vector<Node*> _arrayOfNodes;
  std::vector<Node*>::iterator _it;
  Node* _currentNode;
  
  // TODO: When switching rooms, confirm if a given audio is already playing,
  // so that no unnecessary fade ins/outs are performed.
  std::vector<Audio*> _arrayOfAudios;
  Audio* _defaultFootstep;
  bool _hasDefaultFootstep;
  bool _hasEffects;
  bool _hasEnterEvent;
  int _luaReference;
  
  Room(const Room&);
  void operator=(const Room&);
};
  
}

#endif // DAGON_ROOM_H_
