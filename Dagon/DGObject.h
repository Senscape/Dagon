/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_OBJECT_H
#define	DG_OBJECT_H

#include "DGCommon.h"


// TODO: Each module should have its own type, not here
enum DG_Object_Types {
	DG_OBJ_GENERIC,
	DG_OBJ_AUDIO,
	DG_OBJ_NODE,
	DG_OBJ_ROOM,
	DG_OBJ_SPOT,
	DG_OBJ_TEXTURE,
	DG_OBJ_VIDEO
};

typedef struct {
	unsigned id;
	unsigned type;
	unsigned ref;
	char name[DG_MAX_OBJNAME];
	void* data;
} DG_OBJECT;


// New / Release

DG_OBJECT*		dg_object_new(unsigned type, void* data, size_t num);
DG_OBJECT*		dg_object_new_named(unsigned type, const char* name, void* data, size_t num);
void			dg_object_release(DG_OBJECT* object);

// Checks

DG_BOOL			dg_object_check(DG_OBJECT* object, unsigned type);

// Gets

void*			dg_object_data(DG_OBJECT* object);
unsigned		dg_object_id(DG_OBJECT* object);
const char*		dg_object_name(DG_OBJECT* object);
unsigned		dg_object_refcount(DG_OBJECT* object);

// Sets

void			dg_object_ref(DG_OBJECT* object);
void			dg_object_unref(DG_OBJECT* object);

#endif // DG_OBJECT_H
