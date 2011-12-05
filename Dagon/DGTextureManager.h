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

void		DGTextureManagerInitialize(void);
void		DGTextureManagerTerminate(void);

// State Changes

void		DGTextureManagerBind(DGObject* theTexture);
DGObject*	DGTextureManagerLoad(const char* nameOfTexture);
DGObject*	DGTextureManagerNew(int width, int height, int depth);

#endif // DG_TEXTURE_MANAGER_H
