/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_TEXTURE_MANAGER_H
#define	DG_TEXTURE_MANAGER_H

#include "DGCommon.h"
#include "DGObject.h"


// Init / Release

void		dg_texture_manager_init(void);
void		dg_texture_manager_release(void);

// State Changes

void		dg_texture_manager_bind(DG_OBJECT* texture);
DG_OBJECT*	dg_texture_manager_load(const char* name);
DG_OBJECT*	dg_texture_manager_new(int width, int height, int depth);

#endif // DG_TEXTURE_MANAGER_H
