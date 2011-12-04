/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef dg_array_H
#define	dg_array_H

#include "DGCommon.h"
#include "DGObject.h"

#define	DG_DEF_ARRAYSIZE 500


// TODO: Must implement the enumerator feature here

typedef struct {
	unsigned capacity;
	unsigned count;
	DG_OBJECT** objects;
} DG_ARRAY;


// New / Release

DG_ARRAY*			dg_array_new(unsigned capacity);
void				dg_array_release(DG_ARRAY* array);

// Checks

unsigned			dg_array_count(DG_ARRAY* array);
DG_BOOL				dg_array_has_object(DG_ARRAY* array, DG_OBJECT* object);

// Gets

DG_OBJECT*			dg_array_first(DG_ARRAY* array);
DG_OBJECT*			dg_array_last(DG_ARRAY* array);
DG_OBJECT*			dg_array_get_object(DG_ARRAY* array, unsigned index);
DG_OBJECT*			dg_array_get_object_by_name(DG_ARRAY* array, const char* name);	

// Sets

void				dg_array_add_object(DG_ARRAY* array, DG_OBJECT* object);
void				dg_array_delete_object(DG_ARRAY* array, unsigned index);

#endif // dg_array_H
