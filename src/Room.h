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

#include "Asset.h"
#include "Configurable.h"

#include <memory>
#include <unordered_set>
#include <vector>

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
  bool hasPersistEvent();
  bool hasUnpersistEvent();
  
  // Gets
  std::vector<Audio*> arrayOfAudios();
  Node* currentNode();
  Audio* defaultFootstep();
  SettingCollection effects();
  int enterEvent();
  int persistEvent();
  int unpersistEvent();
  size_t numNodes();

  // Sets
  void setDefaultFootstep(Audio* theFootstep);
  void setEffects(const SettingCollection& theEffects);
  void setPersistEvent(const int luaRef);
  void setUnpersistEvent(const int luaRef);
  
  // State changes
  Audio* addAudio(Audio* anAudio);
  Node* addNode(Node* aNode);
  void beginIteratingNodes();
  bool hasEnterEvent();
  bool iterateNodes();
  Node* iterator();
  void setEnterEvent(int luaReference);
  bool switchTo(Node* theNode);
  void claimAssets();
  void claimAsset(Object* obj);
  void releaseAssets(Room* newRoom);
  
 private:
  SettingCollection _theEffects;
  
  std::vector<Node*> _arrayOfNodes;
  std::vector<Node*>::iterator _it;
  Node* _currentNode;

  std::unordered_set<std::shared_ptr<Asset>, AssetPtrHash> _claimedAssets;
  
  // TODO: When switching rooms, confirm if a given audio is already playing,
  // so that no unnecessary fade ins/outs are performed.
  std::vector<Audio*> _arrayOfAudios;
  Audio* _defaultFootstep;
  bool _hasDefaultFootstep;
  bool _hasEffects;
  bool _hasEnterEvent;
  int _luaReference;
  bool _hasPersistEvent;
  int _persistReference;
  bool _hasUnpersistEvent;
  int _unpersistReference;
  
  Room(const Room&);
  void operator=(const Room&);

  void claimAudio();
};
  
}

#endif // DAGON_ROOM_H_
