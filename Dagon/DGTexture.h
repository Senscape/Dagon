/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_TEXTURE_H
#define	DG_TEXTURE_H

#include "DGCommon.h"
#include "DGObject.h"


// New / Release

DGObject*	DGTextureNew(const char* withName, short andCompressionLevel);
void		DGTextureRelease(DGObject* texture);

// State Changes

void		DGTextureBind(DGObject* texture); // This should be probably handled by the manager
void		DGTextureAppend(DGObject* texture, const char* fileNameToAppend);
void		DGTextureLoad(DGObject* texture, const char* fromFileName, unsigned atIndex);
void		DGTextureLoadFromMemory(DGObject* texture, const unsigned char* dataToLoad);
void		DGTextureSaveToFile(DGObject* texture, const char* fileName);

// Checks

unsigned	DGTextureItemsInBundle(const char* fileName);
unsigned	DGTextureHeight(DGObject* texture);
DGBool		DGTextureIsLoaded(DGObject* texture);
unsigned	DGTextureWidth(DGObject* texture);

// Sets

void		DGTextureClear(DGObject* texture, int width, int height, int depth);

#endif // DG_TEXTURE_H
