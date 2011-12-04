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


void*	dg_alloc(size_t size);
void	dg_free(void* ptr);
void*	dg_realloc(void* ptr, size_t size);
long	dg_reportmem(void);
void*	dg_zeromem(size_t num, size_t size);

#endif // DG_MEMORY_H
