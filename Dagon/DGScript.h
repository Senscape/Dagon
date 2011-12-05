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


void	DGScriptInitialize(void);
void	DGScriptTerminate(void);

void	DGScriptCallback(int theHandler);
void	DGScriptDoLine(const char* aLine);
void    DGScriptHack(void); // Temporary hack to support quick creation of rooms
void	DGScriptLoop(void);
void	DGScriptResume(void);
void	DGScriptRun(void);
void	DGScriptYield(void);

#endif // DG_SCRIPT_H
