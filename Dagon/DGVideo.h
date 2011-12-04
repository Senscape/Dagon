/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_VIDEO_H
#define	DG_VIDEO_H

#include "DGCommon.h"
#include "DGObject.h"


typedef struct {
	int width;
	int	height;
	int bpp;
	int fps;
	unsigned char* data;
} DG_FRAME;

// Init

void		dg_video_init(void);
void        dg_video_terminate(void);

// New / Release

DG_OBJECT*	dg_video_new(const char* name, DG_BOOL autoplay, DG_BOOL loop, DG_BOOL sync);
void		dg_video_release(DG_OBJECT* video);

// Load / Unload

void		dg_video_load(DG_OBJECT* video);
void		dg_video_unload(DG_OBJECT* video);

// Checks

DG_BOOL		dg_video_has_frame(DG_OBJECT* video);
DG_BOOL		dg_video_has_loop(DG_OBJECT* video);
DG_BOOL		dg_video_has_reset(DG_OBJECT* video);
DG_BOOL		dg_video_is_frame_ready(DG_OBJECT* video);
DG_BOOL		dg_video_is_playing(DG_OBJECT* video);
DG_BOOL		dg_video_is_synced(DG_OBJECT* video);

// Gets

DG_FRAME*	dg_video_get_frame(DG_OBJECT* video);

// State Changes

DG_BOOL		dg_video_loop(void);
void		dg_video_play(DG_OBJECT* video);
void		dg_video_pause(DG_OBJECT* video);
void		dg_video_stop(DG_OBJECT* video);

#endif // DG_VIDEO_H
