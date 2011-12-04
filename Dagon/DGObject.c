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

DG_OBJECT* dg_object_new(unsigned type, void* data, size_t num) {
	DG_OBJECT* object;
	
	object = (DG_OBJECT*)dg_alloc(sizeof(DG_OBJECT));
	object->id = id;
	object->type = type;
	object->ref = 0;
	object->data = dg_alloc(num);
	memcpy(object->data, data, num);
	
	id++;
	
	return object;
}

DG_OBJECT* dg_object_new_named(unsigned type, const char* name, void* data, size_t num) {
	DG_OBJECT* object = dg_object_new(type, data, num);
	
	strcpy(object->name, name);
	
	return object;
}

void dg_object_release(DG_OBJECT* object) {
	if (object) {
		object->ref--;
		if (!object->ref) {
			dg_free(object->data);
			dg_free(object);
		}
	}
}

unsigned dg_object_refcount(DG_OBJECT* object) {
	if (object)
		return object->ref;
	
	return 0; // Not wise
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Checks											   /////
////////////////////////////////////////////////////////////////////////////////

DG_BOOL dg_object_check(DG_OBJECT* object, unsigned type) {
	if (object) {
		if (object->type == type)
			return DG_YES;
		else
			return DG_NO;
	}
	else return DG_NO;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Gets												   /////
////////////////////////////////////////////////////////////////////////////////

void* dg_object_data(DG_OBJECT* object) {
	if (object)
		return object->data;
	else
		return NULL;
}

unsigned dg_object_id(DG_OBJECT* object) {
	if (object)
		return object->id;
	else
		return 0;	
}

const char*	dg_object_name(DG_OBJECT* object) {
	if (object)
		return object->name;
	else
		return NULL;
}

void dg_object_ref(DG_OBJECT* object) {
	if (object)
		object->ref++;
}

void dg_object_unref(DG_OBJECT* object) {
	if (object)
		object->ref--;	
}
