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

#include <string>
#include "DGObject.h"

class DGAudio;

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

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
