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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGNode.h"
#include "DGSpot.h"
#include "DGTexture.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGNode::DGNode() {
    _hasBundleName = false;    
    this->setType(DGObjectNode);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGNode::~DGNode() {
    // Note that we only delete spots created by this class, not the ones from the user
    for (_it = _arrayOfSpots.begin(); _it < _arrayOfSpots.end(); _it++) {
        DGSpot* spot = *_it;
        if (spot->hasFlag(DGSpotClass))
            delete spot;
    }
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool DGNode::hasBundleName() {
    return _hasBundleName;
}

bool DGNode::hasSpots() {
    // This should never happen but we check it anyway to
    // avoid any crashes
    return !_arrayOfSpots.empty();
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

DGSpot* DGNode::currentSpot() {
    return *_it;
}

char* DGNode::bundleName() {
    return _bundleName;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void DGNode::setBundleName(const char* name) {
    strncpy(_bundleName, name, DGMaxObjectName);
    _hasBundleName = true;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void DGNode::addCustomLink(unsigned withDirection, int luaHandler) {
    DGAction action;
    
    action.type = DGActionFunction;
    action.cursor = DGCursorUse;
    action.luaHandler = luaHandler;
    
    _link(withDirection, &action);
}

void DGNode::addLink(unsigned int withDirection, DGObject* theTarget) {
    DGAction action;
    
    action.type = DGActionSwitch;
    action.cursor = DGCursorForward;    
    action.target = theTarget;
    
    _link(withDirection, &action);
}

DGSpot* DGNode::addSpot(DGSpot* aSpot) {
    _arrayOfSpots.push_back(aSpot);
    return aSpot;
}

void DGNode::beginIteratingSpots() {
    _it = _arrayOfSpots.begin();
}

bool DGNode::iterateSpots() {
    _it++;
    
    if (_it == _arrayOfSpots.end())
        return false;
    else
        return true;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void DGNode::_link(unsigned int direction, DGAction* action) {
    // We ensure the texture is properly stretched, so we take the default cube size
    // TODO: This setting should be obtained from the DGConfig class
    int minorBound = (int)(DGDefTexSize / 3);
    int majorBound = (int)(DGDefTexSize / 1.5f);
    int offset = (int)(DGDefTexSize/3);
    
    // We always have four corners here, hence eight elements since they are squared spots
    
    std::vector<int> arrayOfCoordinates;
    int coordsNormal[] = {minorBound, minorBound, majorBound, minorBound, majorBound, majorBound, minorBound, majorBound};
    int coordsShiftRight[] = {minorBound+offset, minorBound, majorBound+offset, minorBound, majorBound+offset, majorBound, minorBound+offset, majorBound};
    int coordsShiftLeft[] = {minorBound-offset, minorBound, majorBound-offset, minorBound, majorBound-offset, majorBound, minorBound-offset, majorBound};
    
    DGSpot* newSpot = NULL;
    DGSpot* auxSpot = NULL;
    
    switch (direction) {
        case DGNorth:		
        case DGEast:		
        case DGSouth:	
        case DGWest:
            arrayOfCoordinates.assign(coordsNormal, coordsNormal + 8);
            newSpot = new DGSpot(arrayOfCoordinates, direction, DGSpotClass);
            break;
        case DGNorthEast:
            arrayOfCoordinates.assign(coordsShiftRight, coordsShiftRight + 8);
            newSpot = new DGSpot(arrayOfCoordinates, DGNorth, DGSpotClass);
            arrayOfCoordinates.assign(coordsShiftLeft, coordsShiftLeft + 8);
            auxSpot = new DGSpot(arrayOfCoordinates, DGEast, DGSpotClass);
            break;
        case DGSouthEast:
            arrayOfCoordinates.assign(coordsShiftRight, coordsShiftRight + 8);
            newSpot = new DGSpot(arrayOfCoordinates, DGEast, DGSpotClass);
            arrayOfCoordinates.assign(coordsShiftLeft, coordsShiftLeft + 8);
            auxSpot = new DGSpot(arrayOfCoordinates, DGSouth, DGSpotClass);
            break;
        case DGSouthWest:
            arrayOfCoordinates.assign(coordsShiftRight, coordsShiftRight + 8);
            newSpot = new DGSpot(arrayOfCoordinates, DGSouth, DGSpotClass);
            arrayOfCoordinates.assign(coordsShiftLeft, coordsShiftLeft + 8);
            auxSpot = new DGSpot(arrayOfCoordinates, DGWest, DGSpotClass);
            break;
        case DGNorthWest:
            arrayOfCoordinates.assign(coordsShiftRight, coordsShiftRight + 8);
            newSpot = new DGSpot(arrayOfCoordinates, DGWest, DGSpotClass);
            arrayOfCoordinates.assign(coordsShiftLeft, coordsShiftLeft + 8);
            auxSpot = new DGSpot(arrayOfCoordinates, DGNorth, DGSpotClass);
            break;	
    }
    
    newSpot->setAction(action);
    newSpot->setColor(0); // Color is set automatically
    _arrayOfSpots.push_back(newSpot);
    
    if (auxSpot) {
        auxSpot->setAction(action);
        auxSpot->setColor(0);
        _arrayOfSpots.push_back(auxSpot);
    }
}
