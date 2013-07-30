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
#include "DGSpot.h"
#include "DGVideo.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGSpot::DGSpot(std::vector<int> withArrayOfCoordinates, unsigned int onFace, int withFlags) {
	// Init data with default values
	_color = kColorBlack;
	_flags = withFlags;
	
	_isPlaying = false;
	
	_hasAction = false;
	_hasAudio = false;
	_hasColor = false;
	_hasTexture = false;
	_hasVideo = false;
	
	_arrayOfCoordinates = withArrayOfCoordinates;
	_onFace = onFace;
	
	_xOrigin = 0;
	_yOrigin = 0;
	_zOrder = 0;
    
    _volume = 1.0f;
    
    this->setType(DGObjectSpot);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGSpot::~DGSpot() {
    if (_hasAction)
        delete _actionData;
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool DGSpot::hasAction() {
    return _hasAction;
}

bool DGSpot::hasAudio() {
    return _hasAudio;
}

bool DGSpot::hasColor() {
    return _hasColor;
}

bool DGSpot::hasFlag(int theFlag) {
    if (_flags & theFlag)
        return true;
    else
        return false;
}

bool DGSpot::hasTexture() {
    return _hasTexture;
}

bool DGSpot::hasVideo() {
    return _hasVideo;
}

bool DGSpot::isPlaying() {
    return _isPlaying;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

DGAction* DGSpot::action() {
    return _actionData;
}

DGAudio* DGSpot::audio() {
    return _attachedAudio;
}

int DGSpot::color() {
    return _color;
}

vector<int> DGSpot::arrayOfCoordinates() {
    return _arrayOfCoordinates;
}

unsigned int DGSpot::face() {
    return _onFace;
}

Point DGSpot::origin() {
    Point _origin;
    
    _origin.x = _arrayOfCoordinates[0];
    _origin.y = _arrayOfCoordinates[1];
    
    return _origin;
}

DGTexture* DGSpot::texture() {
    return _attachedTexture;
}

int DGSpot::vertexCount() {
    return (_arrayOfCoordinates.size() / 2.0f);
}

DGVideo* DGSpot::video() {
    return _attachedVideo;
}

float DGSpot::volume() {
    return _volume;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void DGSpot::resize(int width, int height) {
    Point origin;
    
    origin.x = _arrayOfCoordinates[0];
    origin.y = _arrayOfCoordinates[1];
    
    _arrayOfCoordinates.resize(8);
    
    _arrayOfCoordinates[0] = origin.x;
    _arrayOfCoordinates[1] = origin.y;
        
    _arrayOfCoordinates[2] = origin.x + width;
    _arrayOfCoordinates[3] = origin.y;
        
    _arrayOfCoordinates[4] = origin.x + width;
    _arrayOfCoordinates[5] = origin.y + height;
        
    _arrayOfCoordinates[6] = origin.x;
    _arrayOfCoordinates[7] = origin.y + height;
}

void DGSpot::setAction(DGAction* anAction) {
    _actionData = new DGAction;
    memcpy(_actionData, anAction, sizeof(DGAction));
    _hasAction = true;
}

void DGSpot::setAudio(DGAudio* anAudio) {
    _attachedAudio = anAudio;
    _hasAudio = true;
}

void DGSpot::setColor(int aColor) {
    if (!aColor) {
        int r, g, b;
        
        // Make sure we avoid black while generating the color
        r = (rand() % 255) + 1;
        g = (rand() % 255) + 1;
        b = (rand() % 255) + 1;
        
        aColor = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
    }
    
    _color = aColor;
    _hasColor = true;
}

void DGSpot::setOrigin(int x, int y) {
    for (size_t i = 0; i < _arrayOfCoordinates.size(); i += 2) {
        _arrayOfCoordinates[i] += x;
        _arrayOfCoordinates[i + 1] += y;
    }
    
    _xOrigin = x;
    _yOrigin = y;
}

void DGSpot::setTexture(DGTexture* aTexture) {
    _attachedTexture = aTexture;
    _hasTexture = true;
}

void DGSpot::setVolume(float theVolume) {
    _volume = theVolume;
}

void DGSpot::setVideo(DGVideo* aVideo) {
    _attachedVideo = aVideo;
    _hasVideo = true;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void DGSpot::play() {
    _isPlaying = true;
    
    // FIXME: Should start playing only if in the current room
    if (_hasAudio && _attachedAudio->isLoaded())
        _attachedAudio->play();
    
    if (_hasVideo && _attachedVideo->isLoaded()) {
        _attachedVideo->play();
    }
    
    // Hack of sorts but works OK
    if (this->hasFlag(DGSpotLoop))
        _flags = _flags | DGSpotAuto;
}

void DGSpot::stop() {
    _isPlaying = false;
    
    if (_hasAudio)
        _attachedAudio->stop();    
}
