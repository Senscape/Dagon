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
	DGBool can_process;
	char command[DG_MAX_LOGLEN];
	DGBool enabled;
	char input[DG_MAX_LOGLEN];
	short slide;
} DGConsoleState;

void	DGConsoleInitialize(void);
void	DGConsoleTerminate(void);

void	DGConsoleEnable(void);
void	DGConsoleDeleteChar(void);
void	DGConsoleDisable(void);
void	DGConsoleInputChar(const char* aKey);
DGBool	DGConsoleIsEnabled(void);
DGBool	DGConsoleOutput(char* pointerToBuffer);
void	DGConsoleExecute(void);
void	DGConsoleToggleState(void);
void	DGConsoleUpdate(void);

#endif // DG_CONSOLE_H
