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

DG_OBJECT*	dg_texture_new(const char* name, short compression_level);
void		dg_texture_release(DG_OBJECT* texture);

// State Changes

void		dg_texture_bind(DG_OBJECT* texture);
void		dg_texture_append(DG_OBJECT* texture, const char* file);
void		dg_texture_load(DG_OBJECT* texture, const char* file, unsigned index);
void		dg_texture_load_from_memory(DG_OBJECT* texture, const unsigned char* data);
void		dg_texture_save(DG_OBJECT* texture, const char* file);

// Checks

unsigned	dg_texture_count(const char* file);
unsigned	dg_texture_height(DG_OBJECT* texture);
DG_BOOL		dg_texture_is_loaded(DG_OBJECT* texture);
unsigned	dg_texture_width(DG_OBJECT* texture);

// Sets

void		dg_texture_clear(DG_OBJECT* texture, int width, int height, int depth);

#endif // DG_TEXTURE_H
