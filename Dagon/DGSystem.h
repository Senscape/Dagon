/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef dg_SYSTEM_H
#define	dg_SYSTEM_H

#include "DGCommon.h"


void	DGSystemInitialize(void);
void	DGSystemTerminate(void);

void	DGSystemFindPaths(void);
void	DGSystemLock(void);
void	DGSystemRun(void);
void	DGSystemSetTitle(const char* theTitle);
void	DGSystemToggleFullscreen(void);
void	DGSystemUnlock(void);
void	DGSystemUpdate(void);

#endif // DG_SYSTEM_H
