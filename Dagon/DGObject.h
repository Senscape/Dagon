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

enum DGObjectTypes {
    // Note that 0 is used to return an invalid object in Lua
    DGObjectNone = 0,
	DGObjectGeneric,
	DGObjectAudio,
	DGObjectFont,    
	DGObjectNode,
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
    unsigned int _type;
    std::string _name;
    int _luaObject;
    int _retainCount;
    
public:
    DGObject();
    virtual ~DGObject();
    
    // Checks
    
    bool isType(unsigned int typeToCheck);
    
    // Gets
    
    int luaObject();
    const char* name();
    int retainCount();
    unsigned int type();
    
    // Sets
    
    void setLuaObject(int object);
    void setType(unsigned int type);
    void setName(const char* aName);
    
    // State changes
    
    void release();
    void retain();
};

#endif // DG_OBJECT_H
