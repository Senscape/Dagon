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


void		dg_state_init(void);
unsigned	dg_state_current(void);
unsigned	dg_state_last(void);
void		dg_state_return(void);
void		dg_state_set(unsigned new_state);

#endif // DG_STATE_H
