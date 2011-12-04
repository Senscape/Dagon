/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_CONTROL_H
#define	DG_CONTROL_H

#include "DGCommon.h"
#include "DGObject.h"


enum DG_Global_Events {
	DG_EVENT_ENTER_NODE = 1,
	DG_EVENT_LEAVE_NODE,
	DG_EVENT_ENTER_ROOM,
	DG_EVENT_LEAVE_ROOM,
	DG_EVENT_PRE_RENDER,
	DG_EVENT_POST_RENDER,
	DG_EVENT_MOUSE_BUTTON,	
	DG_EVENT_MOUSE_MOVE	
};

void	dg_control_init(void);
void	dg_control_release(void);

DG_BOOL	dg_control_is_running(void);
void	dg_control_feedback(const char* text);
void	dg_control_process_char(const char* key, DG_BOOL modified);
void	dg_control_process_key(const char key);
void	dg_control_process_mouse(short x, short y, DG_BOOL button);
void	dg_control_register_hotkey(int value, const char* line);
void	dg_control_register_global_handler(unsigned type, int ref);
void	dg_control_sleep(int msecs);
void	dg_control_snap(void);
void	dg_control_switch(DG_OBJECT* target);
void	dg_control_update(void);

#endif // DG_CONTROL_H
