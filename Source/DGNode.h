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

#ifndef DG_NODE_H
#define DG_NODE_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGAction.h"
#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class DGAudio;
class DGSpot;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGNode : public DGObject {
    // The bundle name is neither a filename or name of the node,
    // the Texture Manager uses it later to generate the corresponding
    // filenames. This would be the name of the Lua object.
    
    char _bundleName[kMaxObjectName];
    bool _hasBundleName;
    DGNode* _previousNode;
    bool _isSlide;
    int _slideReturn;
    
    DGAudio* _footstep;
    bool _hasFootstep;
    
    std::vector<DGSpot*> _arrayOfSpots;
    std::vector<DGSpot*>::iterator _it;
    std::string _description;    
    
    void _link(unsigned int direction, DGAction* action);
    
public:
    DGNode();
    ~DGNode();
    
    // Checks
    
    bool hasBundleName();
    bool hasFootstep();
    bool hasSpots();
    bool isSlide();
    
    // Gets
    
    char* bundleName();
    const char* description();
    DGSpot* currentSpot();
    DGAudio* footstep();
    DGNode* previousNode();
    int slideReturn();
    
    // Sets
    
    void setBundleName(const char* name);
    void setDescription(const char* description);
    void setFootstep(DGAudio* theFootstep);
    void setPreviousNode(DGNode* node);
    void setSlide(bool enabled);
    void setSlideReturn(int luaHandler);
    
    // State changes
    
    void addCustomLink(unsigned int withDirection, int luaHandler);
    void addLink(unsigned int withDirection, DGObject* theTarget);
    DGSpot* addSpot(DGSpot* aSpot);
    void beginIteratingSpots();
    bool iterateSpots();
};

#endif // DG_NODE_H
