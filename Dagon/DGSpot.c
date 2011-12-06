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
#include "DGSpot.h"

////////////////////////////////////////////////////////////////////////////////
///// Private Data														   /////
////////////////////////////////////////////////////////////////////////////////

typedef struct {
	int color;
	int flags;
	DGAction* action;
	DGObject* texture;
    DGObject* audio_obj;
	DGObject* video_obj;
	
	DGBool is_enabled;
	DGBool is_playing;
	
	DGBool has_action;
	DGBool has_audio;
	DGBool has_color;
	DGBool has_image;
	DGBool has_texture;
	DGBool has_video;
	
	char image[DG_MAX_OBJNAME];
	
	short* coords;
	unsigned elements;
	unsigned face;
	
	// TODO: Setting the origin should affect the coords variable
	short x_origin;
	short y_origin;
	short z_order; // For future use
} DG_SPOT;

////////////////////////////////////////////////////////////////////////////////
///// Private Prototypes												   /////
////////////////////////////////////////////////////////////////////////////////

DG_SPOT* _tospot(DGObject* spot);

////////////////////////////////////////////////////////////////////////////////
///// Standard New / Release											   /////
////////////////////////////////////////////////////////////////////////////////

DGObject* DGSpotNew(short* withArrayOfCoordinates, unsigned sizeOfArray, unsigned onFace, int withFlags) {
	DGObject* spot;
	DG_SPOT spot_data;
	
	// Init data with default values
	spot_data.color = DG_COLOR_BLACK;
	spot_data.flags = withFlags;
	spot_data.action = NULL;
	spot_data.texture = NULL;
	
	spot_data.is_enabled = DG_YES;
	spot_data.is_playing = DG_NO;
	
	spot_data.has_action = DG_NO;
	spot_data.has_audio = DG_NO;
	spot_data.has_color = DG_NO;
	spot_data.has_image = DG_NO;
	spot_data.has_texture = DG_NO;
	spot_data.has_video = DG_NO;
	
	strcpy(spot_data.image, "");
	
	spot_data.coords = DGAlloc(sizeof(short) * sizeOfArray);
	memcpy(spot_data.coords, withArrayOfCoordinates, sizeof(short) * sizeOfArray);
	spot_data.elements = sizeOfArray;
	spot_data.face = onFace;
	
	spot_data.x_origin = 0;
	spot_data.y_origin = 0;
	spot_data.z_order = 0;
	
	// Store data into object and return
	spot = DGObjectNew(DG_OBJECT_SPOT, &spot_data, sizeof(spot_data));
	
	return spot;
}

void DGSpotRelease(DGObject* spot) {
	// If refcount is zero then is means the object was never used, so it's valid
	if (DGObjectReferenceCount(spot) <= 1) {
		DG_SPOT* spot_data = _tospot(spot);
		
		if (spot_data) {
			DGFree(spot_data->coords);
			if (spot_data->has_action)
				DGFree(spot_data->action);

			DGObjectRelease(spot);
		}
	}
	else
		DGObjectRemoveReference(spot);
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - State Changes									   /////
////////////////////////////////////////////////////////////////////////////////

void DGSpotDisable(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->is_enabled = DG_NO;
	}
}

void DGSpotEnable(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->is_enabled = DG_YES;
	}
}

void DGSpotPlay(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->is_playing = DG_YES;
	}
}

void DGSpotStop(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->is_playing = DG_NO;
	}
}

void DGSpotToggle(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		if (spot_data->is_enabled)
			spot_data->is_enabled = DG_NO;
		else
			spot_data->is_enabled = DG_YES;
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Checks											   /////
////////////////////////////////////////////////////////////////////////////////

DGBool	DGSpotHasAction(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->has_action;
	}
	
	return DG_NO;	
}

DGBool	DGSpotHasAudio(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->has_audio;
	}
	
	return DG_NO;
}

DGBool	DGSpotHasColor(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->has_color;		
	}
	
	return DG_NO;
}

DGBool DGSpotHasImage(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->has_image;
	}
	
	return DG_NO;	
}

DGBool DGSpotHasTexture(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->has_texture;
	}
	
	return DG_NO;	
}

DGBool	DGSpotHasVideo(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->has_video;
	}
	
	return DG_NO;	
}

DGBool	DGSpotIsEnabled(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->is_enabled;
	}
	
	return DG_NO;	
}

DGBool	DGSpotIsPlaying(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->is_playing;
	}
	
	return DG_NO;	
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Gets												   /////
////////////////////////////////////////////////////////////////////////////////

DGAction*	DGSpotAction(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->action;
	}
	
	return NULL;
}

DGObject*	DGSpotAudio(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->audio_obj;
	}
	
	return NULL;
}

int	DGSpotColor(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->color;
	}
	
	return DG_COLOR_BLACK;
}

short* DGSpotArrayOfCoordinates(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->coords;
	}
	
	return NULL;	
}

unsigned DGSpotSizeOfArray(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->elements;
	}
	
	return 0;	
}

unsigned DGSpotFace(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->face;
	}
	
	return 0;	
}

const char*	DGSpotImage(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->image;		
	}
	
	return "";
}

DGObject* DGSpotTexture(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->texture;		
	}
	
	return NULL;
}

DGObject* DGSpotVideo(DGObject* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->video_obj;		
	}
	
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Sets												   /////
////////////////////////////////////////////////////////////////////////////////

void DGSpotResize(DGObject* spot, short* withNewArrayOfCoordinates, unsigned sizeOfNewArray) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->coords = DGRealloc(spot_data->coords, sizeof(short) * sizeOfNewArray);
		memcpy(spot_data->coords, withNewArrayOfCoordinates, sizeof(short) * sizeOfNewArray);
		spot_data->elements = sizeOfNewArray;
	}
}

void DGSpotSetAction(DGObject* spot, DGAction* anAction) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->action = DGAlloc(sizeof(DGAction));
		memcpy(spot_data->action, anAction, sizeof(DGAction));
		spot_data->has_action = DG_YES;
	}
}

void DGSpotSetAudio(DGObject* spot, DGObject* anAudio) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->audio_obj = anAudio;
		spot_data->has_audio = DG_YES;
	}
}

void DGSpotSetColor(DGObject* spot, int aColor) {
	// TODO: Must avoid repetition of color (ugly patch)
	static int c = 1;
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		if (!aColor) {
#ifdef DG_PLATFORM_MAC	
			aColor = (arc4random() % 0x0000AA) + (0x002200 * c); // Lesser than blue but avoiding black
#else
			aColor = (rand() % 0x0000AA) + (0x002200 * c); // Lesser than blue but avoiding black
#endif
			c++;
			
			if (c > 10)
				c = 1;
		}
		
		spot_data->color = aColor;
		spot_data->has_color = DG_YES;
	}
}

void DGSpotSetImage(DGObject* spot, const char* anImageFilename) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		strcpy(spot_data->image, anImageFilename);
		spot_data->has_image = DG_YES;
	}
}

void DGSpotSetTexture(DGObject* spot, DGObject* aTexture) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->texture = aTexture;
		spot_data->has_texture = DG_YES;
	}
}

void DGSpotSetVideo(DGObject* spot, DGObject* aVideo) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->video_obj = aVideo;
		spot_data->has_video = DG_YES;
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Private Functions													   /////
////////////////////////////////////////////////////////////////////////////////

DG_SPOT* _tospot(DGObject* spot) {
	if (DGObjectIsType(spot, DG_OBJECT_SPOT)) {
		DG_SPOT* spot_data = (DG_SPOT*)DGObjectData(spot);
		return spot_data;
	}
	else {
		// raise error
		return NULL;
	}
}
