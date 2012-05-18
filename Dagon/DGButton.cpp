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

#include "DGButton.h"
#include "DGConfig.h"
#include "DGFontManager.h"
#include "DGTexture.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGButton::DGButton() {
    config = &DGConfig::getInstance();
    fontManager = &DGFontManager::getInstance();
    
    _color = DGColorBlack;
    _textColor = DGColorWhite;
    
    _hasAction = false;
    _hasColor = false;
    _hasOnHoverTexture = false;
    
    this->setType(DGObjectButton);    
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGButton::~DGButton() {
    if (_hasAction)
        delete _actionData;
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool DGButton::hasAction() {
    return _hasAction;
}

bool DGButton::hasColor() {
    return _hasColor;
}

bool DGButton::hasOnHoverTexture() {
    return _hasOnHoverTexture;
}

bool DGButton::hasFont() {
    return _hasFont;
}

bool DGButton::hasText() {
    return _hasText;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

DGAction* DGButton::action() {
    return _actionData;
}

int DGButton::color() {
    return _color;
}

DGFont* DGButton::font() {
    return _font;
}

DGTexture* DGButton::onHoverTexture() {
    return _attachedOnHoverTexture;
}

const char* DGButton::text() {
    return _text.c_str();
}

int DGButton::textColor() {
    return _textColor;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void DGButton::setAction(DGAction* anAction) {
    _actionData = new DGAction;
    memcpy(_actionData, anAction, sizeof(DGAction));
    _hasAction = true;   
}

void DGButton::setFont(const char* fromFileName, unsigned int heightOfFont) {
    // FIXME: Wrong, this will load many repeated fonts!
    _font = fontManager->load(fromFileName, heightOfFont);
}

void DGButton::setColor(int aColor) {
    if (!aColor) {
        int r, g, b;
        
        // Make sure we avoid black
        r = (rand() % 255) + 1;
        g = (rand() % 255) + 1;
        b = (rand() % 255) + 1;
        
        aColor = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
    }
    
    _color = aColor;
    _hasColor = true;
}

void DGButton::setOnHoverTexture(const char* fromFileName) {
    // TODO: Important! Should determine first if the texture already exists,
    // to avoid repeating resources. Eventually, this would be done with the
    // resource manager.
    DGTexture* texture;
    
    texture = new DGTexture;
    texture->setResource(config->path(DGPathRes, fromFileName));
    texture->load();
    
    _attachedOnHoverTexture = texture;
    _hasOnHoverTexture = true;
}

void DGButton::setText(const char* text){
    _text = text;
}

void DGButton::setTextColor(int aColor) {
    // Note this expects one of the pre-generated colors
    _textColor = aColor;
}
