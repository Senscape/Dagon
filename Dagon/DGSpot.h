/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_SPOT_H
#define	DG_SPOT_H

#include "DGCommon.h"
#include "DGObject.h"


enum DG_Action_Types {
	DG_ACT_CUSTOM,
	DG_ACT_FEEDBACK,
	DG_ACT_SWITCH
};

typedef struct {
	unsigned type;
	int custom_handler;
	char feedback[DG_MAX_FEEDBACK];
	char feedback_audio[DG_MAX_OBJNAME];
	DG_OBJECT* target;
} DG_ACTION;


// New / Release


// TODO: Add auto = false
// TODO: Add structure for retrieving coordinates

DG_OBJECT*		dg_spot_new(short* coords, unsigned elements, unsigned face, int flags);
void			dg_spot_release(DG_OBJECT* spot);

// State Changes

void			dg_spot_disable(DG_OBJECT* spot);
void			dg_spot_enable(DG_OBJECT* spot);
void			dg_spot_play(DG_OBJECT* spot);
void			dg_spot_stop(DG_OBJECT* spot);
void			dg_spot_toggle(DG_OBJECT* spot);

// Checks

DG_BOOL			dg_spot_has_action(DG_OBJECT* spot);
DG_BOOL			dg_spot_has_audio(DG_OBJECT* spot);
DG_BOOL			dg_spot_has_color(DG_OBJECT* spot);
DG_BOOL			dg_spot_has_image(DG_OBJECT* spot);
DG_BOOL			dg_spot_has_texture(DG_OBJECT* spot);
DG_BOOL			dg_spot_has_video(DG_OBJECT* spot);
DG_BOOL			dg_spot_is_enabled(DG_OBJECT* spot);
DG_BOOL			dg_spot_is_playing(DG_OBJECT* spot);

// Gets

DG_ACTION*		dg_spot_action(DG_OBJECT* spot);
DG_OBJECT*      dg_spot_audio(DG_OBJECT* spot);
int				dg_spot_color(DG_OBJECT* spot);
short*			dg_spot_coords(DG_OBJECT* spot);
unsigned		dg_spot_elements(DG_OBJECT* spot);
unsigned		dg_spot_face(DG_OBJECT* spot);
const char*		dg_spot_image(DG_OBJECT* spot);
DG_OBJECT*		dg_spot_texture(DG_OBJECT* spot);
DG_OBJECT*		dg_spot_video(DG_OBJECT* spot);

// Sets

void			dg_spot_resize(DG_OBJECT* spot, short* coords, unsigned elements);
void			dg_spot_set_action(DG_OBJECT* spot, DG_ACTION* action);
void			dg_spot_set_audio(DG_OBJECT* spot, DG_OBJECT* audio);
void			dg_spot_set_color(DG_OBJECT* spot, int color);
void			dg_spot_set_image(DG_OBJECT* spot, const char* file);
void			dg_spot_set_texture(DG_OBJECT* spot, DG_OBJECT* texture);
void			dg_spot_set_video(DG_OBJECT* spot, DG_OBJECT* video);

#endif // DG_SPOT_H
