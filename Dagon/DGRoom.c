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
	DGObject* current_node;
	DGArray* nodes;
} DG_ROOM;

////////////////////////////////////////////////////////////////////////////////
///// Private Prototypes												   /////
////////////////////////////////////////////////////////////////////////////////

DG_ROOM* _toroom(DGObject* room);

////////////////////////////////////////////////////////////////////////////////
///// Standard New / Release											   /////
////////////////////////////////////////////////////////////////////////////////

DGObject* DGRoomNew(const char* withName) {
	DGObject* room;
	DG_ROOM room_data;
	
	room_data.current_node = NULL;
	room_data.nodes = DGArrayNew(0);
	
	room = DGObjectNewNamed(DG_OBJECT_ROOM, withName, &room_data, sizeof(room_data));
	
	return room;
}

void DGRoomRelease(DGObject* room) {
	// If refcount is zero then is means the object was never used, so it's valid
	if (DGObjectReferenceCount(room) <= 1) {	
		DG_ROOM* room_data = _toroom(room);
		
		if (room_data) {
			DGArrayRelease(room_data->nodes);
			DGObjectRelease(room);
		}
	}
	else
		DGObjectRemoveReference(room);
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - State Changes									   /////
////////////////////////////////////////////////////////////////////////////////

void DGRoomSwitchTo(DGObject* room, DGObject* theNode) {
	DG_ROOM* room_data = _toroom(room);
	
	if (room_data) {	
		room_data->current_node = theNode;
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Gets												   /////
////////////////////////////////////////////////////////////////////////////////

DGObject* DGRoomCurrentNode(DGObject* room) {
	DG_ROOM* room_data = _toroom(room);
	
	if (room_data) {	
		return room_data->current_node;
	}
	
	return NULL;
}

DGBool DGRoomGetSpot(DGObject* room, DGObject** pointerToSpot) {
	DG_ROOM* room_data = _toroom(room);
	
	if (room_data) {
		return DGNodeGetSpot(room_data->current_node, pointerToSpot);
	}
	
	return DG_NO;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Sets												   /////
////////////////////////////////////////////////////////////////////////////////

void DGRoomAdd(DGObject* room, DGObject* aNode) {
	DG_ROOM* room_data = _toroom(room);
	
	if (room_data) {	
		DGArrayAddObject(room_data->nodes, aNode);
		if (!room_data->current_node)
			room_data->current_node = aNode;
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Private Functions													   /////
////////////////////////////////////////////////////////////////////////////////

DG_ROOM* _toroom(DGObject* room) {
	if (DGObjectIsType(room, DG_OBJECT_ROOM)) {
		DG_ROOM* room_data = (DG_ROOM*)DGObjectData(room);
		return room_data;
	}
	else {
		// raise error
		return NULL;
	}
}
