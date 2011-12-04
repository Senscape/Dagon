/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_SYSTEM_H
#define	DG_SYSTEM_H

#include "DGCommon.h"


void	dg_system_init(void);
void	dg_system_release(void);

void	dg_system_find_paths(void);
void	dg_system_lock(void);
void	dg_system_run(void);
void	dg_system_set_title(const char* title);
void	dg_system_toggle_fullscreen(void);
void	dg_system_unlock(void);
void	dg_system_update(void);

#endif // DG_SYSTEM_H
