////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DAGON_OBJECT_H_
#define DAGON_OBJECT_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <string>

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class Group;

enum FadeSpeeds {
  kFadeNormal = 250,
  kFadeSlow = 1500,
  kFadeSlowest = 3000,
  kFadeFast = 100,
  kFadeFastest = 10
};

enum FadeTypes {
  kFadeNone,
  kFadeIn,
  kFadeOut
};

enum ObjectTypes {
  // Note that 0 is used to return an invalid object in Lua
  kObjectNone = 0,
  kObjectGeneric,
  kObjectAudio,
  kObjectButton,
  // Cursors are not really separate objects, but for convenience we include
  // them here
  kObjectCursor,
  kObjectFont,
  kObjectGroup,
  kObjectImage,
  kObjectInternalAudio,
  kObjectNode,
  kObjectOverlay,
  kObjectRoom,
  kObjectSave,
  kObjectSlide,
  kObjectSpot,
  kObjectTexture,
  kObjectVideo
};

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class Object {
 public:
  Object();
  virtual ~Object() {};
  
  // Checks
  bool isEnabled();
  bool isFading();
  bool isType(unsigned int typeToCheck);
  
  // Gets
  float fadeLevel();
  int luaObject();
  std::string name();
  int retainCount();
  unsigned int type();
  
  // Sets
  void forceFadeLevel(float level);
  void setDefaultFadeLevel(float level);
  void setFadeLevel(float level);
  void setFadeSpeed(int speed);
  void setGroup(Group* theGroup);
  void setLuaObject(int object);
  void setName(std::string aName);
  void setStatic();
  void setType(unsigned int theType);
  
  // State changes
  void disable(bool forced = false);
  void enable(bool forced = false);
  void fadeIn();
  void fadeOut();
  void release();
  void retain();
  void toggle();
  void updateFade();
  
 private:
  Group* _attachedGroup;
  
  unsigned int _id;
  int _luaObject;
  std::string _name;
  int _retainCount;
  unsigned int _type;
  
  float _defaultFade;
  int _fadeDirection;
  float _fadeLevel;
  float _fadeSpeed;
  float _fadeTarget;
  bool _isEnabled;
  bool _isGrouped;
  bool _isStatic;
  
  Object(const Object&);
  void operator=(const Object&);
};
  
}

#endif // DAGON_OBJECT_H_
