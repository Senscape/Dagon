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

#ifndef DG_OBJECT_H
#define DG_OBJECT_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

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
    DGObjectCursor, // Not really objects
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

// TODO: Lots of stuff to add here: enable, disable, alpha property, etc.

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
