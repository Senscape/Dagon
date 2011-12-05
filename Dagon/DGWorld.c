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
	DGObject* current_room;
	DGObject* current_spot;
	DGArray* rooms;
} DG_WORLD;

////////////////////////////////////////////////////////////////////////////////
///// Standard New / Release											   /////
////////////////////////////////////////////////////////////////////////////////

void DGWorldInitialize() {
	DGLogTrace(DG_MOD_CORE, "Initializing core model...");
	
	DG_WORLD.current_room = NULL;
	
	DG_WORLD.rooms = DGArrayNew(0);
}

void DGWorldTerminate() {
	DGArrayRelease(DG_WORLD.rooms);
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - State Changes									   /////
////////////////////////////////////////////////////////////////////////////////

void DGWorldSwitchTo(DGObject* theTarget) {
	if (theTarget) {
		switch (theTarget->type) {
			case DG_OBJECT_NODE:
				DGRoomSwitchTo(DG_WORLD.current_room, theTarget);
				break;
			case DG_OBJECT_ROOM:
				DG_WORLD.current_room = theTarget;
				break;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
///// Implementation - Gets												   /////
////////////////////////////////////////////////////////////////////////////////

DGObject* DGWorldCurrentNode() {
	return DGRoomCurrentNode(DG_WORLD.current_room);
}

DGObject* DGWorldCurrentRoom() {
	return DG_WORLD.current_room;
}

DGObject* DGWorldCurrentSpot() {
	return DG_WORLD.current_spot;
}

DGBool	DGWorldGetAction(int forColor, DGAction** pointerToAction) {
	return DGNodeGetAction(DGRoomCurrentNode(DG_WORLD.current_room), forColor, pointerToAction);
}

DGBool DGWorldGetSpot(DGObject** pointerToSpot) {
	DGBool ret = DGRoomGetSpot(DG_WORLD.current_room, pointerToSpot);
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Sets												   /////
////////////////////////////////////////////////////////////////////////////////

void DGWorldAdd(DGObject* anObject) {
    if (DGObjectIsType(anObject, DG_OBJECT_NODE)) {
        DGRoomAdd(DG_WORLD.current_room, anObject);
    }
    else if (DGObjectIsType(anObject, DG_OBJECT_ROOM)) {
        DGArrayAddObject(DG_WORLD.rooms, anObject);
        if (!DG_WORLD.current_room)
            DG_WORLD.current_room = anObject;
    }
}

void DGWorldSetCurrentSpot(DGObject* aSpot) {
	DG_WORLD.current_spot = aSpot;
}
