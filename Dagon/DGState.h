/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_STATE_H
#define	DG_STATE_H

#include "DGCommon.h"


void		DGStateInitialize(void);
unsigned	DGStateCurrent(void);
unsigned	DGStateLast(void);
void		DGStateReturn(void);
void		DGStateSet(unsigned theState);

#endif // DG_STATE_H
