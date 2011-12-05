/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGMemory.h"
#include "DGObject.h"


////////////////////////////////////////////////////////////////////////////////
///// Private Data														   /////
////////////////////////////////////////////////////////////////////////////////

static long id = 1;

////////////////////////////////////////////////////////////////////////////////
///// Standard New / Release											   /////
////////////////////////////////////////////////////////////////////////////////

DGObject* DGObjectNew(unsigned withType, void* dataToStore, size_t sizeOfData) {
	DGObject* object;
	
	object = (DGObject*)DGAlloc(sizeof(DGObject));
	object->id = id;
	object->type = withType;
	object->ref = 0;
	object->data = DGAlloc(sizeOfData);
	memcpy(object->data, dataToStore, sizeOfData);
	
	id++;
	
	return object;
}

DGObject* DGObjectNewNamed(unsigned withType, const char* andName, void* dataToStore, size_t sizeOfData) {
	DGObject* object = DGObjectNew(withType, dataToStore, sizeOfData);
	
	strcpy(object->name, andName);
	
	return object;
}

void DGObjectRelease(DGObject* object) {
	if (object) {
		object->ref--;
		if (!object->ref) {
			DGFree(object->data);
			DGFree(object);
		}
	}
}

unsigned DGObjectReferenceCount(DGObject* object) {
	if (object)
		return object->ref;
	
	return 0; // Not wise
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Checks											   /////
////////////////////////////////////////////////////////////////////////////////

DGBool DGObjectIsType(DGObject* object, unsigned typeToCompare) {
	if (object) {
		if (object->type == typeToCompare)
			return DG_YES;
		else
			return DG_NO;
	}
	else return DG_NO;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Gets												   /////
////////////////////////////////////////////////////////////////////////////////

void* DGObjectData(DGObject* object) {
	if (object)
		return object->data;
	else
		return NULL;
}

unsigned DGObjectID(DGObject* object) {
	if (object)
		return object->id;
	else
		return 0;	
}

const char*	DGObjectName(DGObject* object) {
	if (object)
		return object->name;
	else
		return NULL;
}

void DGObjectAddReference(DGObject* object) {
	if (object)
		object->ref++;
}

void DGObjectRemoveReference(DGObject* object) {
	if (object)
		object->ref--;	
}
