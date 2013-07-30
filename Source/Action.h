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

#ifndef DAGON_ACTION_H_
#define DAGON_ACTION_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <string>

#include "DGObject.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum ActionTypes {
	kActionFunction = 100,
	kActionFeed,
	kActionSwitch
};

struct Action {
  unsigned int cursor;
  std::string feed;
  std::string feedAudio;
  bool hasFeedAudio;
  int luaHandler;
  int luaObject;
	DGObject* target;
	unsigned int type;    
};

#endif // DAGON_ACTION_H_
