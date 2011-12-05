/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_ARRAY_H
#define	DG_ARRAY_H

#include "DGCommon.h"
#include "DGObject.h"

#define	DG_DEF_ARRAYSIZE 500


// TODO: Must implement the enumerator feature here

typedef struct {
	unsigned capacity;
	unsigned count;
	DGObject** objects;
} DGArray;


// New / Release

DGArray*			DGArrayNew(unsigned withCapacity);
void				DGArrayRelease(DGArray* array);

// Checks

unsigned			DGArrayCount(DGArray* array);
DGBool				DGArrayHasObject(DGArray* array, DGObject* anObject);

// Gets

DGObject*			DGArrayFirst(DGArray* array);
DGObject*			DGArrayLast(DGArray* array);
DGObject*			DGArrayGetObject(DGArray* array, unsigned atIndex);
DGObject*			DGArrayGetObjectByName(DGArray* array, const char* theName);	

// Sets

void				DGArrayAddObject(DGArray* array, DGObject* anObject);
void				DGArrayDeleteObject(DGArray* array, unsigned atIndex);

#endif // DG_ARRAY_H
