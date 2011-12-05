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
} DGFrame;

// Init

void		DGVideoInitialize(void);
void        DGVideoTerminate(void);

// New / Release

DGObject*	DGVideoNew(const char* withName, DGBool doesAutoplay, DGBool isLoopable, DGBool isSynced);
void		DGVideoRelease(DGObject* video);

// Load / Unload

void		DGVideoLoad(DGObject* video);
void		DGVideoUnload(DGObject* video);

// Checks

DGBool		DGVideoHasFrame(DGObject* video);
DGBool		DGVideoIsFrameReady(DGObject* video);
DGBool		DGVideoIsLoopable(DGObject* video);
DGBool		DGVideoIsReset(DGObject* video);
DGBool		DGVideoIsPlaying(DGObject* video);
DGBool		DGVideoIsSynced(DGObject* video);

// Gets

DGFrame*	DGVideoGetFrame(DGObject* video);

// State Changes

DGBool		DGVideoLoop(void);
void		DGVideoPlay(DGObject* video);
void		DGVideoPause(DGObject* video);
void		DGVideoStop(DGObject* video);

#endif // DG_VIDEO_H
