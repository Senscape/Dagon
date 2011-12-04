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
#include "DGNode.h"
#include "DGRoom.h"


////////////////////////////////////////////////////////////////////////////////
///// Private Data														   /////
////////////////////////////////////////////////////////////////////////////////

typedef struct {
	DG_OBJECT* current_node;
	DG_ARRAY* nodes;
} DG_ROOM;

////////////////////////////////////////////////////////////////////////////////
///// Private Prototypes												   /////
////////////////////////////////////////////////////////////////////////////////

DG_ROOM* _toroom(DG_OBJECT* room);

////////////////////////////////////////////////////////////////////////////////
///// Standard New / Release											   /////
////////////////////////////////////////////////////////////////////////////////

DG_OBJECT* dg_room_new(const char* name) {
	DG_OBJECT* room;
	DG_ROOM room_data;
	
	room_data.current_node = NULL;
	room_data.nodes = dg_array_new(0);
	
	room = dg_object_new_named(DG_OBJ_ROOM, name, &room_data, sizeof(room_data));
	
	return room;
}

void dg_room_release(DG_OBJECT* room) {
	// If refcount is zero then is means the object was never used, so it's valid
	if (dg_object_refcount(room) <= 1) {	
		DG_ROOM* room_data = _toroom(room);
		
		if (room_data) {
			dg_array_release(room_data->nodes);
			dg_object_release(room);
		}
	}
	else
		dg_object_unref(room);
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - State Changes									   /////
////////////////////////////////////////////////////////////////////////////////

void dg_room_switch(DG_OBJECT* room, DG_OBJECT* node) {
	DG_ROOM* room_data = _toroom(room);
	
	if (room_data) {	
		room_data->current_node = node;
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Gets												   /////
////////////////////////////////////////////////////////////////////////////////

DG_OBJECT* dg_room_current_node(DG_OBJECT* room) {
	DG_ROOM* room_data = _toroom(room);
	
	if (room_data) {	
		return room_data->current_node;
	}
	
	return NULL;
}

DG_BOOL dg_room_get_spot(DG_OBJECT* room, DG_OBJECT** spot) {
	DG_ROOM* room_data = _toroom(room);
	
	if (room_data) {
		return dg_node_get_spot(room_data->current_node, spot);
	}
	
	return DG_NO;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Sets												   /////
////////////////////////////////////////////////////////////////////////////////

void dg_room_add_node(DG_OBJECT* room, DG_OBJECT* node) {
	DG_ROOM* room_data = _toroom(room);
	
	if (room_data) {	
		dg_array_add_object(room_data->nodes, node);
		if (!room_data->current_node)
			room_data->current_node = node;
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Private Functions													   /////
////////////////////////////////////////////////////////////////////////////////

DG_ROOM* _toroom(DG_OBJECT* room) {
	if (dg_object_check(room, DG_OBJ_ROOM)) {
		DG_ROOM* room_data = (DG_ROOM*)dg_object_data(room);
		return room_data;
	}
	else {
		// raise error
		return NULL;
	}
}
