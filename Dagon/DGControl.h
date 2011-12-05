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


enum DG_GLOBAL_EVENTS {
	DG_EVENT_ENTER_NODE = 1,
	DG_EVENT_LEAVE_NODE,
	DG_EVENT_ENTER_ROOM,
	DG_EVENT_LEAVE_ROOM,
	DG_EVENT_PRE_RENDER,
	DG_EVENT_POST_RENDER,
	DG_EVENT_MOUSE_BUTTON,	
	DG_EVENT_MOUSE_MOVE	
};

void	DGControlInitialize(void);
void	DGControlTerminate(void);

DGBool	DGControlIsRunning(void);
void	DGControlProcessCharKey(const char* aKey, DGBool isModified);
void	DGControlProcessKey(const char aKey);
void	DGControlProcessMouse(short xPosition, short yPosition, DGBool isButtonPressed);
void	DGControlRegisterHotkey(int theKey, const char* luaCommandToExecute);
void	DGControlRegisterGlobalHandler(unsigned forEvent, int handlerForLua);
void	DGControlShowFeedback(const char* theText);
void	DGControlSleep(int forMilliseconds);
void	DGControlTakeSnapshot(void);
void	DGControlSwitchTo(DGObject* theTarget);
void	DGControlUpdate(void);

#endif // DG_CONTROL_H
