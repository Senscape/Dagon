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

#include "DGObject.h"

using namespace std;

static int globalID = 0;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGObject::DGObject() {
    _id = ++globalID;
    _retainCount = 0;
    
    _defaultFade = 1.0f;
    _fadeLevel = 1.0f;
    _fadeDirection = DGFadeNone;
    _isEnabled = true;
    
    this->setFadeSpeed(DGFadeNormal);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGObject::~DGObject() {
    // Do some checks here and confirm the retainCount is OK
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool DGObject::isEnabled() {
    return _isEnabled;
}

bool DGObject::isFading() {
    return _fadeDirection;
}

bool DGObject::isType(unsigned int typeToCheck) {
    if (_type == typeToCheck)
        return true;
    else
        return false;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

float DGObject::fadeLevel() {
    return _fadeLevel;
}

int DGObject::luaObject() {
    return _luaObject;
}

const char* DGObject::name() {
    return _name.c_str();
}

int DGObject::retainCount() {
    return _retainCount;
}

unsigned int DGObject::type() {
    return _type;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void DGObject::setFadeLevel(float level) {
    _fadeLevel = level;
    _fadeDirection = DGFadeNone;
}

void DGObject::setFadeSpeed(int speed) {
    _fadeSpeed = 1.0f / (float)speed;
}

void DGObject::setLuaObject(int object) {
    _luaObject = object;
}

void DGObject::setName(const char* aName) {
    _name = aName;
}

void DGObject::setType(unsigned int type) {
    _type = type;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void DGObject::disable() {
    _isEnabled = false;
}

void DGObject::enable() {
    _isEnabled = true;
}

void DGObject::fadeIn() {
    // Force object enabled
    _isEnabled = true;
    
    _fadeLevel = 0.0f;
    _fadeDirection = DGFadeIn;
}

void DGObject::fadeOut() {
    _fadeDirection = DGFadeOut;
}

void DGObject::retain() {
    _retainCount++;
}

void DGObject::release() {
    _retainCount--;    
}

void DGObject::toggle() {
    _isEnabled = !_isEnabled;
}

void DGObject::update() {
    switch (_fadeDirection) {
        case DGFadeIn:
            if (_fadeLevel < 1.0f) _fadeLevel += _fadeSpeed;
            else {
                _fadeLevel = 1.0f;
                _fadeDirection = DGFadeNone;
            }
            break;
        case DGFadeOut:
            if (_fadeLevel < 0.0f) {
                _fadeLevel = 0.0f;
                _isEnabled = false;
                _fadeDirection = DGFadeNone;
            }
            else  _fadeLevel -= _fadeSpeed;
            break;
    }
}
