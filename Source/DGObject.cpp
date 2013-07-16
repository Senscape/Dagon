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
    _fadeTarget = 1.0f;
    _fadeDirection = DGFadeNone;
    _isEnabled = true;
    _isStatic = false;
    
    this->setFadeSpeed(DGFadeNormal);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGObject::~DGObject() {
    // TODO: Do some checks here and confirm the retainCount is OK
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

void DGObject::setDefaultFadeLevel(float level) {
    _defaultFade = level;
    
    this->setFadeLevel(_defaultFade);
}

void DGObject::setFadeLevel(float level) {
    _fadeTarget = level;
    
    if (_fadeTarget > _fadeLevel)
        _fadeDirection = DGFadeIn;
    else if (_fadeTarget < _fadeLevel)
        _fadeDirection = DGFadeOut;
    else
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

void DGObject::setStatic() {
    _isStatic = true;
    this->retain();
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
    // Force object to enabled state
    _isEnabled = true;
    
    _fadeLevel = 0.0f;
    _fadeTarget = _defaultFade;
    _fadeDirection = DGFadeIn;
}

void DGObject::fadeOut() {
    _fadeTarget = 0.0f;
    _fadeDirection = DGFadeOut;
}

void DGObject::retain() {
    _retainCount++;
}

void DGObject::release() {
    if (_retainCount > 0 && !_isStatic)
        _retainCount--;    
}

void DGObject::toggle() {
    _isEnabled = !_isEnabled;
}

void DGObject::updateFade() {
    switch (_fadeDirection) {
        case DGFadeIn:
            if (_fadeLevel <_fadeTarget) _fadeLevel += _fadeSpeed;
            else {
                _fadeLevel = _fadeTarget;
                _fadeDirection = DGFadeNone;
            }
            break;
        case DGFadeOut:
            if (_fadeLevel < _fadeTarget) {
                _fadeLevel = _fadeTarget;
                _fadeDirection = DGFadeNone;
                
                if (_fadeLevel <= 0.0f) {
                    _isEnabled = false;
                }
            }
            else  _fadeLevel -= _fadeSpeed;
            break;
    }
}
