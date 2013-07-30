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

#ifndef DG_OBJECT_H
#define DG_OBJECT_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Platform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum DGFadeSpeeds {
    DGFadeNormal = 200,
    DGFadeSlow = 350, 
    DGFadeSlowest = 500,
    DGFadeFast = 100,
    DGFadeFastest = 50
};

enum DGFadeTypes {
    DGFadeNone,
    DGFadeIn,    
    DGFadeOut
};

enum DGObjectTypes {
    // Note that 0 is used to return an invalid object in Lua
    DGObjectNone = 0,
	DGObjectGeneric,
	DGObjectAudio,
    DGObjectButton,
    DGObjectCursor, // Cursors are not really separate objects, but for convenience we include them here
	DGObjectFont,
    DGObjectImage,
	DGObjectNode,
    DGObjectOverlay,
	DGObjectRoom,
    DGObjectSlide,
	DGObjectSpot,
	DGObjectTexture,
	DGObjectVideo
};

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGObject {
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
    bool _isStatic;
    
public:
    DGObject();
    virtual ~DGObject();
    
    // Checks
    
    bool isEnabled();
    bool isFading();
    bool isType(unsigned int typeToCheck);
    
    // Gets
    
    float fadeLevel();
    int luaObject();
    const char* name();
    int retainCount();
    unsigned int type();
    
    // Sets
    
    void setDefaultFadeLevel(float level);
    void setFadeLevel(float level);
    void setFadeSpeed(int speed);
    void setLuaObject(int object);
    void setType(unsigned int type);
    void setName(const char* aName);
    void setStatic();
    
    // State changes
    
    void disable();
    void enable();
    void fadeIn();
    void fadeOut();    
    void release();
    void retain();
    void toggle();
    void updateFade();
};

#endif // DG_OBJECT_H
