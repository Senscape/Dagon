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
#include "DGConfig.h"
#include "DGNode.h"


////////////////////////////////////////////////////////////////////////////////
///// Private Data														   /////
////////////////////////////////////////////////////////////////////////////////

typedef struct {
	DGArray* spots;
	unsigned int marker;
} DG_NODE;

////////////////////////////////////////////////////////////////////////////////
///// Private Prototypes												   /////
////////////////////////////////////////////////////////////////////////////////

DG_NODE* _tonode(DGObject* node);

////////////////////////////////////////////////////////////////////////////////
///// Standard New / Release											   /////
////////////////////////////////////////////////////////////////////////////////

DGObject* DGNodeNew(const char* withName) {
	int i;
	DGObject* node;
	DG_NODE node_data;
	
	node_data.marker = 0;
	node_data.spots = DGArrayNew(0);
	
	// Create six spots for the texture pack feature
	for (i = 0; i < 6; i++) {
		short coords[] = {0, 0, DG_DEF_TEXSIZE, 0, DG_DEF_TEXSIZE, DG_DEF_TEXSIZE, 0, DG_DEF_TEXSIZE};
		DGObject* spot = DGSpotNew(coords, 8, i, DG_NONE);
		DGSpotSetImage(spot, withName);
		DGArrayAddObject(node_data.spots, spot);
	}
	
	node = DGObjectNewNamed(DG_OBJECT_NODE, withName, &node_data, sizeof(node_data));
	
	return node;
}

void DGNodeRelease(DGObject* node) {
	// If refcount is zero then is means the object was never used, so it's valid
	if (DGObjectReferenceCount(node) <= 1) {
		DG_NODE* node_data = _tonode(node);
		
		if (node_data) {
			DGArrayRelease(node_data->spots);
			DGObjectRelease(node);
		}
	}
	else
		DGObjectRemoveReference(node);		
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Gets												   /////
////////////////////////////////////////////////////////////////////////////////

DGBool DGNodeGetAction(DGObject* node, int forColor, DGAction** pointerToAction) {
	DG_NODE* node_data = _tonode(node);
	
	if (node_data) {
		unsigned int i;
		for (i = 0; i < DGArrayCount(node_data->spots); i++) {
			DGObject* spot = DGArrayGetObject(node_data->spots, i);
			if ((DGSpotColor(spot) == forColor) && DGSpotHasAction(spot)) {
				*pointerToAction = DGSpotAction(spot);
				return DG_YES;
			}
		}
	}
	
	return DG_NO;
}

DGBool DGNodeGetSpot(DGObject* node, DGObject** pointerToSpot) {
	DG_NODE* node_data = _tonode(node);
	
	if (node_data) {
		if (node_data->marker < DGArrayCount(node_data->spots)) {
			*pointerToSpot = DGArrayGetObject(node_data->spots, node_data->marker);
			node_data->marker++;
			return DG_YES;
		}
		else {
			node_data->marker = 0;
			return DG_NO;
		}
	}
	
	return DG_NO;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Sets												   /////
////////////////////////////////////////////////////////////////////////////////

void DGNodeAddCustomLink(DGObject* node, unsigned withDirection, int handlerForLua) {
	DG_NODE* node_data = _tonode(node);
	
	if (node_data) {
		short minorBound = (short)(DG_DEF_TEXSIZE / 3);
		short majorBound = (short)(DG_DEF_TEXSIZE / 1.5f);
		short offset = (short)(DG_DEF_TEXSIZE/3);
		
		short coordsNormal[] = {minorBound, minorBound, majorBound, minorBound, majorBound, majorBound, minorBound, majorBound};
		short coordsShiftRight[] = {minorBound+offset, minorBound, majorBound+offset, minorBound, majorBound+offset, majorBound, minorBound+offset, majorBound};
		short coordsShiftLeft[] = {minorBound-offset, minorBound, majorBound-offset, minorBound, majorBound-offset, majorBound, minorBound-offset, majorBound};
		
		DGObject* newSpot = NULL;
		DGObject* auxSpot = NULL;
		DGAction action;
		action.type = DG_ACTION_CUSTOM;
		action.custom_handler = handlerForLua;
		
		switch (withDirection) {
			case DG_NORTH:		
			case DG_EAST:		
			case DG_SOUTH:	
			case DG_WEST:
				newSpot = DGSpotNew(coordsNormal, 8, withDirection, DG_NONE);
				break;
			case DG_NORTHEAST:
				newSpot = DGSpotNew(coordsShiftRight, 8, DG_FRONT, DG_NONE);
				auxSpot = DGSpotNew(coordsShiftLeft, 8, DG_RIGHT, DG_NONE);
				break;
			case DG_SOUTHEAST:
				newSpot = DGSpotNew(coordsShiftRight, 8, DG_RIGHT, DG_NONE);
				auxSpot = DGSpotNew(coordsShiftLeft, 8, DG_BACK, DG_NONE);
				break;
			case DG_SOUTHWEST:
				newSpot = DGSpotNew(coordsShiftRight, 8, DG_BACK, DG_NONE);
				auxSpot = DGSpotNew(coordsShiftLeft, 8, DG_LEFT, DG_NONE);
				break;
			case DG_NORTHWEST:
				newSpot = DGSpotNew(coordsShiftRight, 8, DG_LEFT, DG_NONE);
				auxSpot = DGSpotNew(coordsShiftLeft, 8, DG_FRONT, DG_NONE);
				break;	
		}
		
		DGSpotSetAction(newSpot, &action);
		DGSpotSetColor(newSpot, 0);
		DGArrayAddObject(node_data->spots, newSpot);
		
		if (auxSpot) {
			DGSpotSetAction(auxSpot, &action);
			DGSpotSetColor(auxSpot, 0);
			DGArrayAddObject(node_data->spots, auxSpot);
		}
	}
}

void DGNodeAddLink(DGObject* node, unsigned withDirection, DGObject* theTarget) {
	DG_NODE* node_data = _tonode(node);
	
	if (node_data) {
		short minorBound = (short)(DG_DEF_TEXSIZE / 3);
		short majorBound = (short)(DG_DEF_TEXSIZE / 1.5f);
		short offset = (short)(DG_DEF_TEXSIZE/3);
		
		short coordsNormal[] = {minorBound, minorBound, majorBound, minorBound, majorBound, majorBound, minorBound, majorBound};
		short coordsShiftRight[] = {minorBound+offset, minorBound, majorBound+offset, minorBound, majorBound+offset, majorBound, minorBound+offset, majorBound};
		short coordsShiftLeft[] = {minorBound-offset, minorBound, majorBound-offset, minorBound, majorBound-offset, majorBound, minorBound-offset, majorBound};
		
		DGObject* newSpot = NULL;
		DGObject* auxSpot = NULL;
		DGAction action;
		action.type = DG_ACTION_SWITCH;
		action.target = theTarget;
		
		switch (withDirection) {
			case DG_NORTH:		
			case DG_EAST:		
			case DG_SOUTH:	
			case DG_WEST:
				newSpot = DGSpotNew(coordsNormal, 8, withDirection, DG_NONE);
				break;
			case DG_NORTHEAST:
				newSpot = DGSpotNew(coordsShiftRight, 8, DG_FRONT, DG_NONE);
				auxSpot = DGSpotNew(coordsShiftLeft, 8, DG_RIGHT, DG_NONE);
				break;
			case DG_SOUTHEAST:
				newSpot = DGSpotNew(coordsShiftRight, 8, DG_RIGHT, DG_NONE);
				auxSpot = DGSpotNew(coordsShiftLeft, 8, DG_BACK, DG_NONE);
				break;
			case DG_SOUTHWEST:
				newSpot = DGSpotNew(coordsShiftRight, 8, DG_BACK, DG_NONE);
				auxSpot = DGSpotNew(coordsShiftLeft, 8, DG_LEFT, DG_NONE);
				break;
			case DG_NORTHWEST:
				newSpot = DGSpotNew(coordsShiftRight, 8, DG_LEFT, DG_NONE);
				auxSpot = DGSpotNew(coordsShiftLeft, 8, DG_FRONT, DG_NONE);
				break;	
		}
		
		DGSpotSetAction(newSpot, &action);
		DGSpotSetColor(newSpot, 0);
		DGArrayAddObject(node_data->spots, newSpot);
		
		if (auxSpot) {
			DGSpotSetAction(auxSpot, &action);
			DGSpotSetColor(auxSpot, 0);
			DGArrayAddObject(node_data->spots, auxSpot);
		}
	}
}

void DGNodeAddSpot(DGObject* node, DGObject* aSpot) {
	DG_NODE* node_data = _tonode(node);
	
	if (node_data) {
		DGArrayAddObject(node_data->spots, aSpot);
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Private Functions													   /////
////////////////////////////////////////////////////////////////////////////////

DG_NODE* _tonode(DGObject* node) {
	if (DGObjectIsType(node, DG_OBJECT_NODE)) {
		DG_NODE* node_data = (DG_NODE*)DGObjectData(node);
		return node_data;
	}
	else {
		// raise error
		return NULL;
	}
}
