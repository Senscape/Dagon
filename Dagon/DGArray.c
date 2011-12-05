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

DGArray* DGArrayNew(unsigned withCapacity) {
	DGArray* array = DGAlloc(sizeof(DGArray));
	
	if (!withCapacity)
		withCapacity = DG_DEF_ARRAYSIZE;
	
	array->capacity = withCapacity;
	array->count = 0;
	array->objects = NULL;
	array->objects = DGAlloc(sizeof(DGObject**) * withCapacity);
	
	return array;
}

void DGArrayRelease(DGArray* array) {
	if (array) {
		unsigned c = array->count;
		
		while (c-- > 0) {
			// TODO: Objects NOT being freed, it is the responsibility of each module
			// NOTE: Perhaps can provide a special function...
			//dg_object_release(array->objects[c]);
		}
		
		DGFree(array);
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Checks											   /////
////////////////////////////////////////////////////////////////////////////////

unsigned DGArrayCount(DGArray* array) {
	return array->count;
}

DGBool DGArrayHasObject(DGArray* array, DGObject* anObject) {
	if (array) {
		unsigned i;
		
		for (i = 0; i < array->count; i++)
			if (array->objects[i] == anObject)
				return DG_YES;
	}
	
	return DG_NO;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Gets												   /////
////////////////////////////////////////////////////////////////////////////////

DGObject* DGArrayFirst(DGArray* array) {
	if (array->count == 0)
		return NULL;
	
	return array->objects[0];
}

DGObject* DGArrayLast(DGArray* array) {
	if (array->count == 0)
		return NULL;
	
	return array->objects[array->count - 1];
}

DGObject* DGArrayGetObject(DGArray* array, unsigned atIndex) {
	if (atIndex >= array->count) {
		// TODO: Raise exception
		return NULL;
	}
	
	return array->objects[atIndex];
}

DGObject* DGArrayGetObjectByName(DGArray* array, const char* theName) {
	unsigned int i;
	
	for (i = 0; i < array->count; i++)
		if (strcmp(array->objects[i]->name, theName) == 0)
			return array->objects[i];
	
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Sets												   /////
////////////////////////////////////////////////////////////////////////////////

void DGArrayAddObject(DGArray* array, DGObject* anObject) {
	if (anObject == NULL) {
		// TODO: Raise exception
		return;
	}
	
	array->objects[array->count] = anObject;
	array->count++;
	
	if (array->count > array->capacity) {
		// WARNING: There seems to be some problems with this...
		DGObject** newObjects;
		array->capacity = array->capacity * 2;
		newObjects = DGRealloc(array->objects, sizeof(DGObject**) * array->capacity);
		array->objects = newObjects;
	}
}

void DGArrayDeleteObject(DGArray* array, unsigned atIndex) {
	unsigned int i, last = array->count - 1;
	
	if (atIndex > last)
		return;
	
	for (i = atIndex; i < last - 1; i++)
		array->objects[i] = array->objects[i+1];
	
	DGObjectRelease(array->objects[last]);
	
	array->count--;
}
