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

enum DGFades {
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
    bool _isEnabled;
    
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
    
    void setLuaObject(int object);
    void setType(unsigned int type);
    void setName(const char* aName);
    
    // State changes
    
    void disable();
    void enable();
    void fadeIn();
    void fadeOut();    
    void release();
    void retain();
    void toggle();
    void update();
};

#endif // DG_OBJECT_H
