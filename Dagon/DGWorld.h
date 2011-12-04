/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_WORLD_H
#define	DG_WORLD_H

#include "DGCommon.h"
#include "DGObject.h"
#include "DGNode.h"
#include "DGRoom.h"
#include "DGSpot.h"


// New / Release

void		dg_world_new(void); // Should be init...
void		dg_world_release(void);

// State Changes

void		dg_world_switch(DG_OBJECT* target);

// Gets

DG_OBJECT*	dg_world_current_node(void);
DG_OBJECT*	dg_world_current_room(void);
DG_OBJECT*	dg_world_current_spot(void);
DG_BOOL		dg_world_get_action(int color, DG_ACTION** action);
DG_BOOL		dg_world_get_spot(DG_OBJECT** spot);

// Sets

void		dg_world_add_node(DG_OBJECT* node);
void		dg_world_add_room(DG_OBJECT* room);
void		dg_world_set_current_spot(DG_OBJECT* spot);

#endif // DG_WORLD_H
