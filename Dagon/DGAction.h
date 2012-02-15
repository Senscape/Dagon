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

class DGAudio;
class DGObject;

// Enum begins at 100 to avoid conflicts with objects
enum DGActionTypes {
	DGActionCustom = 100,
	DGActionFeedback,
	DGActionSwitch
};

typedef struct {
	unsigned int type;
    std::string feedback;
    DGAudio* feedbackAudio;
    int luaHandler;
	DGObject* target;
} DGAction;

#endif // DG_ACTION_H
