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

#ifndef DG_ACTION_H
#define DG_ACTION_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class DGObject;

// Enum begins at 100 to avoid conflicts with objects
enum DGActionTypes {
	DGActionFunction = 100,
	DGActionFeed,
	DGActionSwitch
};

typedef struct {
    unsigned int cursor;
    char feed[DGMaxFeedLength];
    char feedAudio[DGMaxFileLength];
    bool hasFeedAudio;
    int luaHandler;
    int luaObject;
	DGObject* target;
	unsigned int type;    
} DGAction;

#endif // DG_ACTION_H
