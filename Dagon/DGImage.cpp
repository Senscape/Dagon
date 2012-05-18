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

#include "DGConfig.h"
#include "DGImage.h"
#include "DGTexture.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGImage::DGImage() {
    config = &DGConfig::getInstance();
    
    _rect.origin.x = 0;
    _rect.origin.y = 0; 
    _rect.size.width = 0;
    _rect.size.height = 0;
    
    _hasTexture = false;
    
    this->setType(DGObjectImage);    
}

DGImage::DGImage(const char* fromFileName) {
    config = &DGConfig::getInstance();
    
    this->setTexture(fromFileName);
    if (_attachedTexture->isLoaded()) {
        _rect.origin.x = 0;
        _rect.origin.y = 0; 
        
        _rect.size.width = _attachedTexture->width();
        _rect.size.height = _attachedTexture->height();
        
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
// Implementation - State changes
////////////////////////////////////////////////////////////

void DGImage::setPosition(int x, int y) {
    _rect.origin.x = x;
    _rect.origin.y = y;
    
    _calculateCoordinates();    
}

void DGImage::setSize(int width, int height) {
    _rect.size.width = width;
    _rect.size.height = height;
    
    _calculateCoordinates(); 
}

void DGImage::setTexture(const char* fromFileName) {
    // TODO: Important! Should determine first if the texture already exists,
    // to avoid repeating resources. Eventually, this would be done with the
    // resource manager.
    DGTexture* texture;
    
    texture = new DGTexture;
    texture->setResource(config->path(DGPathRes, fromFileName));
    texture->load();
    
    _attachedTexture = texture;
    _hasTexture = true;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

int* DGImage::arrayOfCoordinates() {
    return _arrayOfCoordinates;
}

void DGImage::move(int offsetX, int offsetY) {
    _rect.origin.x += offsetX;
    _rect.origin.y += offsetY;
    
    _calculateCoordinates();
}

void DGImage::scale(float factor) {
    _rect.size.width *= factor;
    _rect.size.height *= factor;
    
    _calculateCoordinates();    
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void DGImage::_calculateCoordinates() {
    _arrayOfCoordinates[0] = _rect.origin.x;
    _arrayOfCoordinates[1] = _rect.origin.y;
    
    _arrayOfCoordinates[2] = _rect.origin.x + _rect.size.width;
    _arrayOfCoordinates[3] = _rect.origin.y;
    
    _arrayOfCoordinates[4] = _rect.origin.x + _rect.size.width;
    _arrayOfCoordinates[5] = _rect.origin.y + _rect.size.height;
    
    _arrayOfCoordinates[6] = _rect.origin.x;
    _arrayOfCoordinates[7] = _rect.origin.y + _rect.size.height;    
}
