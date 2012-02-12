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

class DGSpot;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGNode : public DGObject {
    // The bundle name is neither a filename or name of the node,
    // the Texture Manager uses it later to generate the corresponding
    // filename. This would be the name of the Lua object.
    
    char _bundleName[DGMaxObjectName];
    bool _hasBundleName;
    
    std::vector<DGSpot*> _arrayOfSpots;
    std::vector<DGSpot*>::iterator _it;
    
    void _link(unsigned int direction, DGAction* action);
    
    // Footstep?
    
public:
    DGNode();
    ~DGNode();
    
    // Checks
    
    bool hasBundleName();
    bool hasSpots();
    
    // Gets
    
    char* bundleName();
    DGSpot* currentSpot();
    
    // Sets
    
    void setBundleName(const char* name);
    
    // State changes
    
    void addCustomLink(unsigned int withDirection, int luaHandler);
    void addLink(unsigned int withDirection, DGObject* theTarget);
    DGSpot* addSpot(DGSpot* aSpot);
    void beginIteratingSpots();
    bool iterateSpots();
};

#endif // DG_NODE_H
