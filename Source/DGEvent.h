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
#define	DG_EVENT_H

enum DGGlobalEvents { // For Lua
	DGEventEnterNode = 0x101, // Must implement
	DGEventLeaveNode = 0x102, // Must implement
	DGEventEnterRoom = 0x103, // Must implement
	DGEventLeaveRoom = 0x104, // Must implement
	DGEventPreRender = 0x105, // Must implement
	DGEventPostRender = 0x106,
	DGEventResize = 0x107
};

enum kKeyEvents {
    DGEventKeyDown = 0x201,
    DGEventKeyModified = 0x202,
    DGEventKeyUp = 0x203
};

enum DGMouseEvents {
    DGEventMouseDown = 0x301,
    DGEventMouseDrag = 0x302,
    DGEventMouseMove = 0x303,
    DGEventMouseUp = 0x304,
    DGEventMouseRightDown = 0x305,
    DGEventMouseRightUp = 0x306
};

#endif // DG_EVENT_H