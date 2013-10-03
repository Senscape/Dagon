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

#include "Object.h"

namespace dagon {

static int globalID = 0;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Object::Object() {
  _retainCount = 0;
  _defaultFade = 1.0f;
  _fadeDirection = kFadeNone;
  _fadeLevel = 1.0f;
  _fadeTarget = 1.0f;
  _isEnabled = true;
  _isStatic = false;
  _id = ++globalID;
  this->setFadeSpeed(kFadeNormal);
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool Object::isEnabled() {
  return _isEnabled;
}

bool Object::isFading() {
  return _fadeDirection;
}

bool Object::isType(unsigned int typeToCheck) {
  if (_type == typeToCheck) {
    return true;
  } else {
    return false;
  }
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

float Object::fadeLevel() {
  return _fadeLevel;
}

int Object::luaObject() {
  return _luaObject;
}

std::string Object::name() {
  return _name;
}

int Object::retainCount() {
  return _retainCount;
}

unsigned int Object::type() {
  return _type;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void Object::setDefaultFadeLevel(float level) {
  _defaultFade = level;
  this->setFadeLevel(_defaultFade);
}

void Object::setFadeLevel(float level) {
  _fadeTarget = level;
  if (_fadeTarget > _fadeLevel) {
    _fadeDirection = kFadeIn;
  } else if (_fadeTarget < _fadeLevel) {
    _fadeDirection = kFadeOut;
  } else {
    _fadeDirection = kFadeNone;
  }
}

void Object::setFadeSpeed(int speed) {
  _fadeSpeed = 1.0f / static_cast<float>(speed);
}

void Object::setLuaObject(int object) {
  _luaObject = object;
}

void Object::setName(std::string aName) {
  _name = aName;
}

void Object::setType(unsigned int theType) {
  _type = theType;
}

void Object::setStatic() {
  _isStatic = true;
  this->retain();
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void Object::disable() {
  _isEnabled = false;
}

void Object::enable() {
  _isEnabled = true;
}

void Object::fadeIn() {
  // Force object to enabled state
  _isEnabled = true;
  _fadeLevel = 0;
  _fadeTarget = _defaultFade;
  _fadeDirection = kFadeIn;
}

void Object::fadeOut() {
  _fadeTarget = 0;
  _fadeDirection = kFadeOut;
}

void Object::retain() {
  _retainCount++;
}

void Object::release() {
  if (_retainCount > 0 && !_isStatic)
    _retainCount--;
}

void Object::toggle() {
  _isEnabled = !_isEnabled;
}

void Object::updateFade() {
  if (_fadeDirection == kFadeIn) {
    if (_fadeLevel <_fadeTarget) {
      _fadeLevel += _fadeSpeed;
    } else {
      _fadeLevel = _fadeTarget;
      _fadeDirection = kFadeNone;
    }
  } else if (_fadeDirection == kFadeOut) {
    if (_fadeLevel < _fadeTarget) {
      _fadeLevel = _fadeTarget;
      _fadeDirection = kFadeNone;
      if (_fadeLevel <= 0)
        _isEnabled = false;
    } else {
      _fadeLevel -= _fadeSpeed;
    }
  }
}
  
}
