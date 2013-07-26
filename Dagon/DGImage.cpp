////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2012 Senscape s.r.l.
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

#include "DGConfig.h"
#include "DGImage.h"
#include "DGTexture.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGImage::DGImage() {
    config = &DGConfig::getInstance();
    
    _rect = DGZeroRect;
    
    _hasTexture = false;
    
    this->setType(DGObjectImage);    
}

DGImage::DGImage(const char* fromFileName) {
    config = &DGConfig::getInstance();
    
    this->setTexture(fromFileName);
    if (_attachedTexture->isLoaded()) {
        _rect.origin = DGZeroPoint;
        
        _rect.size = DGMakeSize(_attachedTexture->width(), _attachedTexture->height());
        
        _calculateCoordinates();
    }
    
    this->setType(DGObjectImage);    
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGImage::~DGImage() {
    // Nothing to do here
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool DGImage::hasTexture() {
    return _hasTexture;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

DGPoint DGImage::position() {
    return _rect.origin;    
}

DGSize DGImage::size() {
    return _rect.size;
}

DGTexture* DGImage::texture() {
    return _attachedTexture;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

float* DGImage::arrayOfCoordinates() {
    return _arrayOfCoordinates;
}

void DGImage::move(float offsetX, float offsetY) {
	DGMoveRect(_rect, offsetX, offsetY);
    
    _calculateCoordinates();
}

void DGImage::scale(float factor) {
    DGScaleRect(_rect, factor);
    
    _calculateCoordinates();
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void DGImage::setPosition(float x, float y) {
    _rect.origin = DGMakePoint(x, y);
    
    _calculateCoordinates();
}

void DGImage::setSize(float width, float height) {
    _rect.size = DGMakeSize(width, height);
    
    _calculateCoordinates(); 
}

void DGImage::setTexture(const char* fromFileName) {
    // TODO: Important! Should determine first if the texture already exists,
    // to avoid repeating resources. Eventually, this would be done via the
    // resource manager.
    DGTexture* texture;
    
    texture = new DGTexture;
    texture->setResource(config->path(DGPathRes, fromFileName, DGObjectImage));
    texture->load();
    
    _attachedTexture = texture;
    _hasTexture = true;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void DGImage::_calculateCoordinates() {
    _arrayOfCoordinates[0] = DGMinX(_rect);
    _arrayOfCoordinates[1] = DGMinY(_rect);
    
    _arrayOfCoordinates[2] = DGMaxX(_rect);
    _arrayOfCoordinates[3] = DGMinY(_rect);
    
    _arrayOfCoordinates[4] = DGMaxX(_rect);
    _arrayOfCoordinates[5] = DGMaxY(_rect);
    
    _arrayOfCoordinates[6] = DGMinX(_rect);
    _arrayOfCoordinates[7] = DGMaxY(_rect);   
}
