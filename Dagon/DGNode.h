/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_NODE_H
#define	DG_NODE_H

#include "DGCommon.h"
#include "DGObject.h"
#include "DGSpot.h"


// New / Release

DGObject*	DGNodeNew(const char* withName);
void		DGNodeRelease(DGObject* node);

// Gets

DGBool		DGNodeGetAction(DGObject* node, int forColor, DGAction** pointerToAction);
DGBool		DGNodeGetSpot(DGObject* node, DGObject** pointerToSpot);

// Sets

void		DGNodeAddCustomLink(DGObject* node, unsigned withDirection, int handlerForLua);
void		DGNodeAddLink(DGObject* node, unsigned withDirection, DGObject* theTarget);
void		DGNodeAddSpot(DGObject* node, DGObject* aSpot);

#endif // DG_NODE_H
