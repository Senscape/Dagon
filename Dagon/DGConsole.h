/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_CONSOLE_H
#define	DG_CONSOLE_H

#include "DGCommon.h"


typedef struct {
	DG_BOOL can_process;
	char command[DG_MAX_LOGLEN];
	DG_BOOL enabled;
	char input[DG_MAX_LOGLEN];
	short slide;
} DG_CONSOLE_STATE;

void	dg_console_init(void);
void	dg_console_release(void);

void	dg_console_enable(void);
void	dg_console_delete(void);
void	dg_console_disable(void);
void	dg_console_input(const char* key);
DG_BOOL	dg_console_is_enabled(void);
DG_BOOL	dg_console_output(char* buffer);
void	dg_console_execute(void);
void	dg_console_toggle(void);
void	dg_console_update(void);

#endif // DG_CONSOLE_H
