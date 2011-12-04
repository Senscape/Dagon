/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_SCRIPT_H
#define	DG_SCRIPT_H

#include "DGCommon.h"


void	dg_script_init(void);
void	dg_script_release(void);

void	dg_script_callback(int ref);
void	dg_script_doline(const char* line);
void    dg_script_hack(void);
void	dg_script_loop(void);
void	dg_script_resume(void);
void	dg_script_run(void);
void	dg_script_yield(void);

#endif // DG_SCRIPT_H
