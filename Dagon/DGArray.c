/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGArray.h"
#include "DGMemory.h"


////////////////////////////////////////////////////////////////////////////////
///// Standard New / Release											   /////
////////////////////////////////////////////////////////////////////////////////

DG_ARRAY* dg_array_new(unsigned capacity) {
	DG_ARRAY* array = dg_alloc(sizeof(DG_ARRAY));
	
	if (!capacity)
		capacity = DG_DEF_ARRAYSIZE;
	
	array->capacity = capacity;
	array->count = 0;
	array->objects = NULL;
	array->objects = dg_alloc(sizeof(DG_OBJECT**) * capacity);
	
	return array;
}

void dg_array_release(DG_ARRAY* array) {
	if (array) {
		unsigned c = array->count;
		
		while (c-- > 0) {
			// TODO: Objects NOT being free, it is the responsibility of each module
			// NOTE: Perhaps can provide a special function...
			//dg_object_release(array->objects[c]);
		}
		
		dg_free(array);
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Checks											   /////
////////////////////////////////////////////////////////////////////////////////

unsigned dg_array_count(DG_ARRAY* array) {
	return array->count;
}

DG_BOOL dg_array_has_object(DG_ARRAY* array, DG_OBJECT* object) {
	if (array) {
		unsigned i;
		
		for (i = 0; i < array->count; i++)
			if (array->objects[i] == object)
				return DG_YES;
	}
	
	return DG_NO;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Gets												   /////
////////////////////////////////////////////////////////////////////////////////

DG_OBJECT* dg_array_first(DG_ARRAY* array) {
	if (array->count == 0)
		return NULL;
	
	return array->objects[0];
}

DG_OBJECT* dg_array_last(DG_ARRAY* array) {
	if (array->count == 0)
		return NULL;
	
	return array->objects[array->count - 1];
}

DG_OBJECT* dg_array_get_object(DG_ARRAY* array, unsigned index) {
	if (index >= array->count) {
		// TODO: Raise exception
		return NULL;
	}
	
	return array->objects[index];
}

DG_OBJECT* dg_array_get_object_by_name(DG_ARRAY* array, const char* name) {
	unsigned int i;
	
	for (i = 0; i < array->count; i++)
		if (strcmp(array->objects[i]->name, name) == 0)
			return array->objects[i];
	
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Sets												   /////
////////////////////////////////////////////////////////////////////////////////

void dg_array_add_object(DG_ARRAY* array, DG_OBJECT* object) {
	if (object == NULL) {
		// TODO: Raise exception
		return;
	}
	
	array->objects[array->count] = object;
	array->count++;
	
	if (array->count > array->capacity) {
		// WARNING: There seems to be some problems with this...
		DG_OBJECT** newObjects;
		array->capacity = array->capacity * 2;
		newObjects = dg_realloc(array->objects, sizeof(DG_OBJECT**) * array->capacity);
		array->objects = newObjects;
	}
}

void dg_array_delete_object(DG_ARRAY* array, unsigned index) {
	unsigned int i, last = array->count - 1;
	
	if (index > last)
		return;
	
	for (i = index; i < last - 1; i++)
		array->objects[i] = array->objects[i+1];
	
	dg_object_release(array->objects[last]);
	
	array->count--;
}
