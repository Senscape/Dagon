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
	DG_ACTION* action;
	DG_OBJECT* texture;
    DG_OBJECT* audio_obj;
	DG_OBJECT* video_obj;
	
	DG_BOOL is_enabled;
	DG_BOOL is_playing;
	
	DG_BOOL has_action;
	DG_BOOL has_audio;
	DG_BOOL has_color;
	DG_BOOL has_image;
	DG_BOOL has_texture;
	DG_BOOL has_video;
	
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

DG_SPOT* _tospot(DG_OBJECT* spot);

////////////////////////////////////////////////////////////////////////////////
///// Standard New / Release											   /////
////////////////////////////////////////////////////////////////////////////////

DG_OBJECT* dg_spot_new(short* coords, unsigned elements, unsigned face, int flags) {
	DG_OBJECT* spot;
	DG_SPOT spot_data;
	
	// Init data with default values
	spot_data.color = DG_COLOR_BLACK;
	spot_data.flags = flags;
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
	
	spot_data.coords = dg_alloc(sizeof(short) * elements);
	memcpy(spot_data.coords, coords, sizeof(short) * elements);
	spot_data.elements = elements;
	spot_data.face = face;
	
	spot_data.x_origin = 0;
	spot_data.y_origin = 0;
	spot_data.z_order = 0;
	
	// Store data into object and return
	spot = dg_object_new(DG_OBJ_SPOT, &spot_data, sizeof(spot_data));
	
	return spot;
}

void dg_spot_release(DG_OBJECT* spot) {
	// If refcount is zero then is means the object was never used, so it's valid
	if (dg_object_refcount(spot) <= 1) {
		DG_SPOT* spot_data = _tospot(spot);
		
		if (spot_data) {
			dg_free(spot_data->coords);
			if (spot_data->has_action)
				dg_free(spot_data->action);

			dg_object_release(spot);
		}
	}
	else
		dg_object_unref(spot);
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - State Changes									   /////
////////////////////////////////////////////////////////////////////////////////

void dg_spot_disable(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->is_enabled = DG_NO;
	}
}

void dg_spot_enable(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->is_enabled = DG_YES;
	}
}

void dg_spot_play(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->is_playing = DG_YES;
	}
}

void dg_spot_stop(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->is_playing = DG_NO;
	}
}

void dg_spot_toggle(DG_OBJECT* spot) {
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

DG_BOOL	dg_spot_has_action(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->has_action;
	}
	
	return DG_NO;	
}

DG_BOOL	dg_spot_has_audio(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->has_audio;
	}
	
	return DG_NO;
}

DG_BOOL	dg_spot_has_color(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->has_color;		
	}
	
	return DG_NO;
}

DG_BOOL dg_spot_has_image(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->has_image;
	}
	
	return DG_NO;	
}

DG_BOOL dg_spot_has_texture(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->has_texture;
	}
	
	return DG_NO;	
}

DG_BOOL	dg_spot_has_video(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->has_video;
	}
	
	return DG_NO;	
}

DG_BOOL	dg_spot_is_enabled(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->is_enabled;
	}
	
	return DG_NO;	
}

DG_BOOL	dg_spot_is_playing(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->is_playing;
	}
	
	return DG_NO;	
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Gets												   /////
////////////////////////////////////////////////////////////////////////////////

DG_ACTION*	dg_spot_action(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->action;
	}
	
	return NULL;
}

DG_OBJECT*	dg_spot_audio(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->audio_obj;
	}
	
	return NULL;
}

int	dg_spot_color(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->color;
	}
	
	return DG_COLOR_BLACK;
}

short* dg_spot_coords(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->coords;
	}
	
	return NULL;	
}

unsigned dg_spot_elements(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->elements;
	}
	
	return 0;	
}

unsigned dg_spot_face(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->face;
	}
	
	return 0;	
}

const char*	dg_spot_image(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->image;		
	}
	
	return "";
}

DG_OBJECT* dg_spot_texture(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->texture;		
	}
	
	return NULL;
}

DG_OBJECT* dg_spot_video(DG_OBJECT* spot) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		return spot_data->video_obj;		
	}
	
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Sets												   /////
////////////////////////////////////////////////////////////////////////////////

void dg_spot_resize(DG_OBJECT* spot, short* coords, unsigned elements) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->coords = dg_realloc(spot_data->coords, sizeof(short) * elements);
		memcpy(spot_data->coords, coords, sizeof(short) * elements);
		spot_data->elements = elements;
	}
}

void dg_spot_set_action(DG_OBJECT* spot, DG_ACTION* action) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->action = dg_alloc(sizeof(DG_ACTION));
		memcpy(spot_data->action, action, sizeof(DG_ACTION));
		spot_data->has_action = DG_YES;
	}
}

void dg_spot_set_audio(DG_OBJECT* spot, DG_OBJECT* audio) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->audio_obj = audio;
		spot_data->has_audio = DG_YES;
	}
}

void dg_spot_set_color(DG_OBJECT* spot, int color) {
	// TODO: Must avoid repetition of color (ugly patch)
	static int c = 1;
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		if (!color) {
#ifdef DG_PLATFORM_MAC	
			color = (arc4random() % 0x0000AA) + (0x002200 * c); // Lesser than blue but avoiding black
#else
			color = (rand() % 0x0000AA) + (0x002200 * c); // Lesser than blue but avoiding black
#endif
			c++;
			
			if (c > 10)
				c = 1;
		}
		
		spot_data->color = color;
		spot_data->has_color = DG_YES;
	}
}

void dg_spot_set_image(DG_OBJECT* spot, const char* file) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		strcpy(spot_data->image, file);
		spot_data->has_image = DG_YES;
	}
}

void dg_spot_set_texture(DG_OBJECT* spot, DG_OBJECT* texture) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->texture = texture;
		spot_data->has_texture = DG_YES;
	}
}

void dg_spot_set_video(DG_OBJECT* spot, DG_OBJECT* video) {
	DG_SPOT* spot_data = _tospot(spot);
	
	if (spot_data) {
		spot_data->video_obj = video;
		spot_data->has_video = DG_YES;
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Private Functions													   /////
////////////////////////////////////////////////////////////////////////////////

DG_SPOT* _tospot(DG_OBJECT* spot) {
	if (dg_object_check(spot, DG_OBJ_SPOT)) {
		DG_SPOT* spot_data = (DG_SPOT*)dg_object_data(spot);
		return spot_data;
	}
	else {
		// raise error
		return NULL;
	}
}
