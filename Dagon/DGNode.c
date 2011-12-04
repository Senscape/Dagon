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
	DG_ARRAY* spots;
	unsigned int marker;
} DG_NODE;

////////////////////////////////////////////////////////////////////////////////
///// Private Prototypes												   /////
////////////////////////////////////////////////////////////////////////////////

DG_NODE* _tonode(DG_OBJECT* node);

////////////////////////////////////////////////////////////////////////////////
///// Standard New / Release											   /////
////////////////////////////////////////////////////////////////////////////////

DG_OBJECT* dg_node_new(const char* name) {
	int i;
	DG_OBJECT* node;
	DG_NODE node_data;
	
	node_data.marker = 0;
	node_data.spots = dg_array_new(0);
	
	// Create six spots for the texture pack feature
	for (i = 0; i < 6; i++) {
		short coords[] = {0, 0, DG_DEF_TEXSIZE, 0, DG_DEF_TEXSIZE, DG_DEF_TEXSIZE, 0, DG_DEF_TEXSIZE};
		DG_OBJECT* spot = dg_spot_new(coords, 8, i, DG_NONE);
		dg_spot_set_image(spot, name);
		dg_array_add_object(node_data.spots, spot);
	}
	
	node = dg_object_new_named(DG_OBJ_NODE, name, &node_data, sizeof(node_data));
	
	return node;
}

void dg_node_release(DG_OBJECT* node) {
	// If refcount is zero then is means the object was never used, so it's valid
	if (dg_object_refcount(node) <= 1) {
		DG_NODE* node_data = _tonode(node);
		
		if (node_data) {
			dg_array_release(node_data->spots);
			dg_object_release(node);
		}
	}
	else
		dg_object_unref(node);		
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Gets												   /////
////////////////////////////////////////////////////////////////////////////////

DG_BOOL dg_node_get_action(DG_OBJECT* node, int color, DG_ACTION** action) {
	DG_NODE* node_data = _tonode(node);
	
	if (node_data) {
		unsigned int i;
		for (i = 0; i < dg_array_count(node_data->spots); i++) {
			DG_OBJECT* spot = dg_array_get_object(node_data->spots, i);
			if ((dg_spot_color(spot) == color) && dg_spot_has_action(spot)) {
				*action = dg_spot_action(spot);
				return DG_YES;
			}
		}
	}
	
	return DG_NO;
}

DG_BOOL dg_node_get_spot(DG_OBJECT* node, DG_OBJECT** spot) {
	DG_NODE* node_data = _tonode(node);
	
	if (node_data) {
		if (node_data->marker < dg_array_count(node_data->spots)) {
			*spot = dg_array_get_object(node_data->spots, node_data->marker);
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

void dg_node_add_custom_link(DG_OBJECT* node, unsigned direction, int handler) {
	DG_NODE* node_data = _tonode(node);
	
	if (node_data) {
		short minorBound = (short)(DG_DEF_TEXSIZE / 3);
		short majorBound = (short)(DG_DEF_TEXSIZE / 1.5f);
		short offset = (short)(DG_DEF_TEXSIZE/3);
		
		short coordsNormal[] = {minorBound, minorBound, majorBound, minorBound, majorBound, majorBound, minorBound, majorBound};
		short coordsShiftRight[] = {minorBound+offset, minorBound, majorBound+offset, minorBound, majorBound+offset, majorBound, minorBound+offset, majorBound};
		short coordsShiftLeft[] = {minorBound-offset, minorBound, majorBound-offset, minorBound, majorBound-offset, majorBound, minorBound-offset, majorBound};
		
		DG_OBJECT* newSpot = NULL;
		DG_OBJECT* auxSpot = NULL;
		DG_ACTION action;
		action.type = DG_ACT_CUSTOM;
		action.custom_handler = handler;
		
		switch (direction) {
			case DG_NORTH:		
			case DG_EAST:		
			case DG_SOUTH:	
			case DG_WEST:
				newSpot = dg_spot_new(coordsNormal, 8, direction, DG_NONE);
				break;
			case DG_NORTHEAST:
				newSpot = dg_spot_new(coordsShiftRight, 8, DG_FRONT, DG_NONE);
				auxSpot = dg_spot_new(coordsShiftLeft, 8, DG_RIGHT, DG_NONE);
				break;
			case DG_SOUTHEAST:
				newSpot = dg_spot_new(coordsShiftRight, 8, DG_RIGHT, DG_NONE);
				auxSpot = dg_spot_new(coordsShiftLeft, 8, DG_BACK, DG_NONE);
				break;
			case DG_SOUTHWEST:
				newSpot = dg_spot_new(coordsShiftRight, 8, DG_BACK, DG_NONE);
				auxSpot = dg_spot_new(coordsShiftLeft, 8, DG_LEFT, DG_NONE);
				break;
			case DG_NORTHWEST:
				newSpot = dg_spot_new(coordsShiftRight, 8, DG_LEFT, DG_NONE);
				auxSpot = dg_spot_new(coordsShiftLeft, 8, DG_FRONT, DG_NONE);
				break;	
		}
		
		dg_spot_set_action(newSpot, &action);
		dg_spot_set_color(newSpot, 0);
		dg_array_add_object(node_data->spots, newSpot);
		
		if (auxSpot) {
			dg_spot_set_action(auxSpot, &action);
			dg_spot_set_color(auxSpot, 0);
			dg_array_add_object(node_data->spots, auxSpot);
		}
	}
}

void dg_node_add_link(DG_OBJECT* node, unsigned direction, DG_OBJECT* target) {
	DG_NODE* node_data = _tonode(node);
	
	if (node_data) {
		short minorBound = (short)(DG_DEF_TEXSIZE / 3);
		short majorBound = (short)(DG_DEF_TEXSIZE / 1.5f);
		short offset = (short)(DG_DEF_TEXSIZE/3);
		
		short coordsNormal[] = {minorBound, minorBound, majorBound, minorBound, majorBound, majorBound, minorBound, majorBound};
		short coordsShiftRight[] = {minorBound+offset, minorBound, majorBound+offset, minorBound, majorBound+offset, majorBound, minorBound+offset, majorBound};
		short coordsShiftLeft[] = {minorBound-offset, minorBound, majorBound-offset, minorBound, majorBound-offset, majorBound, minorBound-offset, majorBound};
		
		DG_OBJECT* newSpot = NULL;
		DG_OBJECT* auxSpot = NULL;
		DG_ACTION action;
		action.type = DG_ACT_SWITCH;
		action.target = target;
		
		switch (direction) {
			case DG_NORTH:		
			case DG_EAST:		
			case DG_SOUTH:	
			case DG_WEST:
				newSpot = dg_spot_new(coordsNormal, 8, direction, DG_NONE);
				break;
			case DG_NORTHEAST:
				newSpot = dg_spot_new(coordsShiftRight, 8, DG_FRONT, DG_NONE);
				auxSpot = dg_spot_new(coordsShiftLeft, 8, DG_RIGHT, DG_NONE);
				break;
			case DG_SOUTHEAST:
				newSpot = dg_spot_new(coordsShiftRight, 8, DG_RIGHT, DG_NONE);
				auxSpot = dg_spot_new(coordsShiftLeft, 8, DG_BACK, DG_NONE);
				break;
			case DG_SOUTHWEST:
				newSpot = dg_spot_new(coordsShiftRight, 8, DG_BACK, DG_NONE);
				auxSpot = dg_spot_new(coordsShiftLeft, 8, DG_LEFT, DG_NONE);
				break;
			case DG_NORTHWEST:
				newSpot = dg_spot_new(coordsShiftRight, 8, DG_LEFT, DG_NONE);
				auxSpot = dg_spot_new(coordsShiftLeft, 8, DG_FRONT, DG_NONE);
				break;	
		}
		
		dg_spot_set_action(newSpot, &action);
		dg_spot_set_color(newSpot, 0);
		dg_array_add_object(node_data->spots, newSpot);
		
		if (auxSpot) {
			dg_spot_set_action(auxSpot, &action);
			dg_spot_set_color(auxSpot, 0);
			dg_array_add_object(node_data->spots, auxSpot);
		}
	}
}

void dg_node_add_spot(DG_OBJECT* node, DG_OBJECT* spot) {
	DG_NODE* node_data = _tonode(node);
	
	if (node_data) {
		dg_array_add_object(node_data->spots, spot);
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Private Functions													   /////
////////////////////////////////////////////////////////////////////////////////

DG_NODE* _tonode(DG_OBJECT* node) {
	if (dg_object_check(node, DG_OBJ_NODE)) {
		DG_NODE* node_data = (DG_NODE*)dg_object_data(node);
		return node_data;
	}
	else {
		// raise error
		return NULL;
	}
}
