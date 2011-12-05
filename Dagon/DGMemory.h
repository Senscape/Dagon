/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_MEMORY_H
#define	DG_MEMORY_H

#include "DGCommon.h"


void*	DGAlloc(size_t size);
void	DGFree(void* ptr);
void*	DGRealloc(void* ptr, size_t size);
long	DGReportMem(void);
void*	DGZeroMem(size_t num, size_t size);

#endif // DG_MEMORY_H
