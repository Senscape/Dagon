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

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class DGAudio;
class DGNode;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGRoom : public DGObject {
    std::vector<DGNode*> _arrayOfNodes;
    DGNode* _currentNode;
    
    // NOTE: When switching rooms, confirm if a given audio is already playing, so that no unnecessary fade ins/outs
    // are performed.
    
    std::vector<DGAudio*> _arrayOfAudios;
    DGAudio* _defaultFootstep;
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
    
    std::vector<DGAudio*> arrayOfAudios();
    DGNode* currentNode();
    DGAudio* defaultFootstep();
    int effectsFlags();
    
    // Sets
    
    void setDefaultFootstep(DGAudio* theFootstep);
    void setEffects(int theEffectFlags);
    
    // State changes
    
    DGAudio* addAudio(DGAudio* anAudio);
    DGNode* addNode(DGNode* aNode);
    bool switchTo(DGNode* theNode);
};

#endif // DG_ROOM_H
