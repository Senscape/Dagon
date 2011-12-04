/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_TIMER_H
#define	DG_TIMER_H

#include "DGCommon.h"


unsigned	dg_timer_start(int secs, int ref);
void        dg_timer_pause(unsigned);
void        dg_timer_stop(unsigned);

int         dg_timer_check(void);

#endif // DG_TIMER_H