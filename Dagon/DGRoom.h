/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_ROOM_H
#define	DG_ROOM_H

#include "DGCommon.h"
#include "DGObject.h"


// New / Release

DGObject*	DGRoomNew(const char* withName);
void		DGRoomRelease(DGObject* room);

// State Changes

void		DGRoomSwitchTo(DGObject* room, DGObject* theNode);

// Gets

DGObject*	DGRoomCurrentNode(DGObject* room);
DGBool		DGRoomGetSpot(DGObject* room, DGObject** pointerToSpot);

// Sets

void		DGRoomAdd(DGObject* room, DGObject* aNode);

#endif // DG_ROOM_H
