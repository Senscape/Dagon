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
#include "DGLog.h"
#include "DGWorld.h"


////////////////////////////////////////////////////////////////////////////////
///// Private Data														   /////
////////////////////////////////////////////////////////////////////////////////

struct {
	DG_OBJECT* current_room;
	DG_OBJECT* current_spot;
	DG_ARRAY* rooms;
} DG_WORLD;

////////////////////////////////////////////////////////////////////////////////
///// Standard New / Release											   /////
////////////////////////////////////////////////////////////////////////////////

void dg_world_new() {
	dg_log_trace(DG_MOD_CORE, "Initializing core model...");
	
	DG_WORLD.current_room = NULL;
	
	DG_WORLD.rooms = dg_array_new(0);
}

void dg_world_release() {
	dg_array_release(DG_WORLD.rooms);
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - State Changes									   /////
////////////////////////////////////////////////////////////////////////////////

void dg_world_switch(DG_OBJECT* target) {
	if (target) {
		switch (target->type) {
			case DG_OBJ_NODE:
				dg_room_switch(DG_WORLD.current_room, target);
				break;
			case DG_OBJ_ROOM:
				DG_WORLD.current_room = target;
				break;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
///// Implementation - Gets												   /////
////////////////////////////////////////////////////////////////////////////////

DG_OBJECT* dg_world_current_node() {
	return dg_room_current_node(DG_WORLD.current_room);
}

DG_OBJECT* dg_world_current_room() {
	return DG_WORLD.current_room;
}

DG_OBJECT* dg_world_current_spot() {
	return DG_WORLD.current_spot;
}

DG_BOOL	dg_world_get_action(int color, DG_ACTION** action) {
	return dg_node_get_action(dg_room_current_node(DG_WORLD.current_room), color, action);
}

DG_BOOL dg_world_get_spot(DG_OBJECT** spot) {
	DG_BOOL ret = dg_room_get_spot(DG_WORLD.current_room, spot);
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Sets												   /////
////////////////////////////////////////////////////////////////////////////////

void dg_world_add_node(DG_OBJECT* node) {
	dg_room_add_node(DG_WORLD.current_room, node);
}

void dg_world_add_room(DG_OBJECT* room) {
	dg_array_add_object(DG_WORLD.rooms, room);
	if (!DG_WORLD.current_room)
		DG_WORLD.current_room = room;
}

void dg_world_set_current_spot(DG_OBJECT* spot) {
	DG_WORLD.current_spot = spot;
}
