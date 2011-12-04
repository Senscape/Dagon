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

DG_OBJECT*	dg_room_new(const char* name);
void		dg_room_release(DG_OBJECT* room);

// State Changes

void		dg_room_switch(DG_OBJECT* room, DG_OBJECT* node);

// Gets

DG_OBJECT*	dg_room_current_node(DG_OBJECT* room);
DG_BOOL		dg_room_get_spot(DG_OBJECT* room, DG_OBJECT** spot);

// Sets

void		dg_room_add_node(DG_OBJECT* room, DG_OBJECT* node);

#endif // DG_ROOM_H
