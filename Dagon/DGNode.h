/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef dg_node_H
#define	dg_node_H

#include "DGCommon.h"
#include "DGObject.h"
#include "DGSpot.h"


// New / Release

DG_OBJECT*	dg_node_new(const char* name);
void		dg_node_release(DG_OBJECT* node);

// Gets

DG_BOOL		dg_node_get_action(DG_OBJECT* node, int color, DG_ACTION** action);
DG_BOOL		dg_node_get_spot(DG_OBJECT* node, DG_OBJECT** spot);

// Sets

void		dg_node_add_custom_link(DG_OBJECT* node, unsigned direction, int handler);
void		dg_node_add_link(DG_OBJECT* node, unsigned direction, DG_OBJECT* target);
void		dg_node_add_spot(DG_OBJECT* node, DG_OBJECT* spot);

#endif // dg_node_H
