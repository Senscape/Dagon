////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
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
    
    DGAudio* arrayOfAudios();
    DGNode* currentNode();
    DGAudio* defaultFootstep();
    int effectsFlags();
    
    // Sets
    
    void setDefaultFoostep(DGAudio* theFootstep);
    void setEffects(int theEffectFlags);
    
    // State changes
    
    DGAudio* addAudio(DGAudio* anAudio);
    DGNode* addNode(DGNode* aNode);
    bool switchTo(DGNode* theNode);
};

#endif // DG_ROOM_H
