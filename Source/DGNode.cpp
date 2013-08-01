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

#include "Audio.h"
#include "DGNode.h"
#include "DGSpot.h"
#include "DGTexture.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGNode::DGNode() {
    _hasBundleName = false;
    _hasFootstep = false;
    _isSlide = false;
    _slideReturn = 0;
    
    this->setType(DGObjectNode);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGNode::~DGNode() {
    // Note that we only delete spots automatically created by this class, not the ones by the user
	_it = _arrayOfSpots.begin();

    while (_it != _arrayOfSpots.end()) {
        /*DGSpot* spot = (*_it);
		// FIXME: This causes a crash on Windows
        if (spot->hasFlag(DGSpotClass))
            delete spot;*/

		_it++;
    }
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool DGNode::hasBundleName() {
    return _hasBundleName;
}

bool DGNode::hasFootstep() {
    return _hasFootstep;
}

bool DGNode::hasSpots() {
    // This should never happen (zero spots) but we check it anyway to
    // avoid any crashes
    return !_arrayOfSpots.empty();
}

bool DGNode::isSlide() {
    return _isSlide;
}

int DGNode::slideReturn() {
    return _slideReturn;
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

Audio* DGNode::footstep() {
    return _footstep;
}

const char* DGNode::description() {
    return _description.c_str();
}

DGNode* DGNode::previousNode() {
    return _previousNode;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void DGNode::setBundleName(const char* name) {
    strncpy(_bundleName, name, kMaxObjectName);
    _hasBundleName = true;
}

void DGNode::setDescription(const char* description) {
    _description = description;
}

void DGNode::setFootstep(Audio* theFootstep) {
    _footstep = theFootstep;
    _hasFootstep = true;
}

void DGNode::setPreviousNode(DGNode* node) {
    _previousNode = node;
}

void DGNode::setSlide(bool enabled) {
    _isSlide = enabled;
}

void DGNode::setSlideReturn(int luaHandler) {
    _slideReturn = luaHandler;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void DGNode::addCustomLink(unsigned withDirection, int luaHandler) {
    Action action;
    
    action.type = kActionFunction;
    action.cursor = kCursorForward;
    action.luaHandler = luaHandler;
    
    _link(withDirection, &action);
}

void DGNode::addLink(unsigned int withDirection, DGObject* theTarget) {
    Action action;
    
    action.type = kActionSwitch;
    action.cursor = kCursorForward;    
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

void DGNode::_link(unsigned int direction, Action* action) {
    // We ensure the texture is properly stretched, so we take the default cube size
    // TODO: This setting should be obtained directly from the Config class
    int minorBound = (int)(kDefTexSize / 3);
    int majorBound = (int)(kDefTexSize / 1.5f);
    int offset = (int)(kDefTexSize/3);
    
    // We always have four corners here, hence eight elements since they are squared spots
    
    std::vector<int> arrayOfCoordinates;
    int coordsNormal[] = {minorBound, minorBound, majorBound, minorBound, majorBound, majorBound, minorBound, majorBound};
    int coordsShiftRight[] = {minorBound+offset, minorBound, majorBound+offset, minorBound,
        majorBound+offset, majorBound, minorBound+offset, majorBound};
    int coordsShiftLeft[] = {minorBound-offset, minorBound, majorBound-offset, minorBound,
        majorBound-offset, majorBound, minorBound-offset, majorBound};
    
    DGSpot* newSpot = NULL;
    DGSpot* auxSpot = NULL;
    
    switch (direction) {
        case kNorth:		
        case kEast:		
        case kSouth:	
        case kWest:
            arrayOfCoordinates.assign(coordsNormal, coordsNormal + 8);
            newSpot = new DGSpot(arrayOfCoordinates, direction, DGSpotClass);
            break;
        case kNorthEast:
            arrayOfCoordinates.assign(coordsShiftRight, coordsShiftRight + 8);
            newSpot = new DGSpot(arrayOfCoordinates, kNorth, DGSpotClass);
            arrayOfCoordinates.assign(coordsShiftLeft, coordsShiftLeft + 8);
            auxSpot = new DGSpot(arrayOfCoordinates, kEast, DGSpotClass);
            break;
        case kSouthEast:
            arrayOfCoordinates.assign(coordsShiftRight, coordsShiftRight + 8);
            newSpot = new DGSpot(arrayOfCoordinates, kEast, DGSpotClass);
            arrayOfCoordinates.assign(coordsShiftLeft, coordsShiftLeft + 8);
            auxSpot = new DGSpot(arrayOfCoordinates, kSouth, DGSpotClass);
            break;
        case kSouthWest:
            arrayOfCoordinates.assign(coordsShiftRight, coordsShiftRight + 8);
            newSpot = new DGSpot(arrayOfCoordinates, kSouth, DGSpotClass);
            arrayOfCoordinates.assign(coordsShiftLeft, coordsShiftLeft + 8);
            auxSpot = new DGSpot(arrayOfCoordinates, kWest, DGSpotClass);
            break;
        case kNorthWest:
            arrayOfCoordinates.assign(coordsShiftRight, coordsShiftRight + 8);
            newSpot = new DGSpot(arrayOfCoordinates, kWest, DGSpotClass);
            arrayOfCoordinates.assign(coordsShiftLeft, coordsShiftLeft + 8);
            auxSpot = new DGSpot(arrayOfCoordinates, kNorth, DGSpotClass);
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