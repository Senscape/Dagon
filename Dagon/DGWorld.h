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

void		DGWorldInitialize(void);
void		DGWorldTerminate(void);

// State Changes

void		DGWorldSwitchTo(DGObject* theTarget);

// Gets

DGObject*	DGWorldCurrentNode(void);
DGObject*	DGWorldCurrentRoom(void);
DGObject*	DGWorldCurrentSpot(void);
DGBool		DGWorldGetAction(int forColor, DGAction** pointerToAction);
DGBool		DGWorldGetSpot(DGObject** pointerToSpot);

// Sets

void		DGWorldAdd(DGObject* anObject);
void		DGWorldSetCurrentSpot(DGObject* aSpot); // This is an ugly hack

#endif // DG_WORLD_H
