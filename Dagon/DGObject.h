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
enum DG_OBJECT_TYPES {
	DG_OBJECT_GENERIC,
	DG_OBJECT_AUDIO,
	DG_OBJECT_NODE,
	DG_OBJECT_ROOM,
	DG_OBJECT_SPOT,
	DG_OBJECT_TEXTURE,
	DG_OBJECT_VIDEO
};

typedef struct {
	unsigned id;
	unsigned type;
	unsigned ref;
	char name[DG_MAX_OBJNAME];
	void* data;
} DGObject;


// New / Release

DGObject*		DGObjectNew(unsigned withType, void* dataToStore, size_t sizeOfData);
DGObject*		DGObjectNewNamed(unsigned withType, const char* andName, void* dataToStore, size_t sizeOfData);
void			DGObjectRelease(DGObject* object);

// Checks

DGBool			DGObjectIsType(DGObject* object, unsigned typeToCompare);

// Gets

void*			DGObjectData(DGObject* object);
unsigned		DGObjectID(DGObject* object);
const char*		DGObjectName(DGObject* object);
unsigned		DGObjectReferenceCount(DGObject* object);

// Sets

void			DGObjectAddReference(DGObject* object);
void			DGObjectRemoveReference(DGObject* object);

#endif // DG_OBJECT_H
