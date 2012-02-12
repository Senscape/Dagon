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

enum DGActionTypes {
	DGActionCustom,
	DGActionFeedback,
	DGActionSwitch
};

typedef struct {
	unsigned int type;
	int luaHandler;
    std::string feedback;
	DGAudio* feedbackAudio;
	DGObject* target;
} DGAction;

#endif // DG_ACTION_H
