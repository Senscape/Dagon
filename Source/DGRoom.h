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

#ifndef DG_ROOM_H
#define DG_ROOM_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Platform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class Audio;
class Node;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGRoom : public DGObject {
    std::vector<Node*> _arrayOfNodes;
    std::vector<Node*>::iterator _it;    
    Node* _currentNode;
    
    // NOTE: When switching rooms, confirm if a given audio is already playing, so that no unnecessary fade ins/outs
    // are performed.
    
    std::vector<Audio*> _arrayOfAudios;
    Audio* _defaultFootstep;
    int _effectsFlags;
    bool _hasDefaultFootstep;
    
public:
    DGRoom();
    ~DGRoom();
    
    // Checks
    
    bool hasAudios();
    bool hasDefaultFootstep();
    bool hasNodes();
    
    // Gets
    
    std::vector<Audio*> arrayOfAudios();
    Node* currentNode();
    Audio* defaultFootstep();
    int effectsFlags();
    
    // Sets
    
    void setDefaultFootstep(Audio* theFootstep);
    void setEffects(int theEffectFlags);
    
    // State changes
    
    Audio* addAudio(Audio* anAudio);
    Node* addNode(Node* aNode);
    void beginIteratingNodes();
    bool iterateNodes();
    Node* iterator();
    bool switchTo(Node* theNode);
};

#endif // DG_ROOM_H
