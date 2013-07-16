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

#include "DGAudio.h"
#include "DGNode.h"
#include "DGRoom.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGRoom::DGRoom() {
    _hasDefaultFootstep = false;
    this->setType(DGObjectRoom);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGRoom::~DGRoom() {
    // Nothing to do here
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool DGRoom::hasAudios() {
    return !_arrayOfAudios.empty();
}

bool DGRoom::hasDefaultFootstep() {
    return _hasDefaultFootstep;
}

bool DGRoom::hasNodes() {
    // We invert the return value to comply with our method
    return !_arrayOfNodes.empty();
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

vector<DGAudio*> DGRoom::arrayOfAudios() {
    return _arrayOfAudios;
}

DGNode* DGRoom::currentNode() {
    return _currentNode;
}

DGAudio* DGRoom::defaultFootstep() {
    return _defaultFootstep;
}

int DGRoom::effectsFlags() {
    return _effectsFlags;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void DGRoom::setDefaultFootstep(DGAudio* theFootstep) {
    _defaultFootstep = theFootstep;
    _hasDefaultFootstep = true;
}

void DGRoom::setEffects(int theEffectFlags) {
    _effectsFlags = theEffectFlags;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

DGAudio* DGRoom::addAudio(DGAudio* anAudio) {
    _arrayOfAudios.push_back(anAudio);
    return anAudio; 
}

DGNode* DGRoom::addNode(DGNode* aNode) {
    if (_arrayOfNodes.empty())
        _currentNode = aNode;
    
    _arrayOfNodes.push_back(aNode);
    
    return aNode;   
}

bool DGRoom::switchTo(DGNode* theNode) {
    vector<DGNode*>::iterator it;

    for (it = _arrayOfNodes.begin(); it != _arrayOfNodes.end(); it++) {
        if (*it == theNode) {
            // Node found, OK
            _currentNode = theNode;
            
            return true;
        }
    }

    return false;
}
