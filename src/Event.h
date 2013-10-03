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

#ifndef DG_EVENT_H
#define DG_EVENT_H

namespace dagon {

enum DGGlobalEvents { // For Lua
  EventEnterNode = 0x101, // Must implement
  EventLeaveNode = 0x102, // Must implement
  EventEnterRoom = 0x103, // Must implement
  EventLeaveRoom = 0x104, // Must implement
  EventPreRender = 0x105, // Must implement
  EventPostRender = 0x106,
  EventResize = 0x107
};

enum kKeyEvents {
  EventKeyDown = 0x201,
  EventKeyModified = 0x202,
  EventKeyUp = 0x203
};

enum DGMouseEvents {
  EventMouseDown = 0x301,
  EventMouseDrag = 0x302,
  EventMouseMove = 0x303,
  EventMouseUp = 0x304,
  EventMouseRightDown = 0x305,
  EventMouseRightUp = 0x306
};
  
}

#endif // DG_EVENT_H
