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

// Enum here begins at 100 to avoid conflicts with objects IDs
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
