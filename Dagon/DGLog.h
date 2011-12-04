/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_LOG_H
#define	DG_LOG_H

#include "DGCommon.h"


enum DG_Log_Modules {
	DG_MOD_AUDIO,
	DG_MOD_CAMERA,
	DG_MOD_CONFIG,
	DG_MOD_CONTROL,
	DG_MOD_CORE,
	DG_MOD_EFFECTS,
	DG_MOD_FONT,
	DG_MOD_RENDER,
	DG_MOD_SCRIPT,
	DG_MOD_STATE,
	DG_MOD_SYSTEM,
	DG_MOD_TEXTURE,
	DG_MOD_VIDEO,
	DG_MOD_NONE
};

enum DG_Log_Types {
	DG_LOG_TRACE,
	DG_LOG_ERROR,
	DG_LOG_WARNING
};

void			dg_log_init(void);
void			dg_log_release(void);
unsigned int	dg_log_color(int index);
DG_BOOL			dg_log_history(unsigned int* index, char* buffer);
void			dg_log_command(int module, const char* string, ...);
void			dg_log_info(int module, const char* string, ...);
void			dg_log_error(int module, const char* string, ...);
void			dg_log_trace(int module, const char* string, ...);
void			dg_log_warning(int module, const char* string, ...);

#endif // DG_LOG_H
