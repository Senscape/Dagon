/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_RENDER_H
#define	DG_RENDER_H

#include "DGCommon.h"
#include "DGObject.h"


// Init / Release

void		DGRenderInitialize(void);
void		DGRenderTerminate(void);

// State Changes

void		DGRenderBegin(DGBool usingTextures);
void		DGRenderBlendNextUpdate(void);
void		DGRenderShowCutscene(DGObject* withVideo);
void		DGRenderDrawDust(void);
void		DGRenderDrawCursor(short xPosition, short yPosition);
void		DGRenderDrawOverlay(short* withArrayOfCoordinates, DGBool isInverted);
void		DGRenderDrawPolygon(short* withArrayOfCoordinates, short sizeOfArray, short onFace, DGBool isInverted);
void		DGRenderDrawSplash(void);
void		DGRenderEnd(void);
void		DGRenderSetColor(int color);
void		DGRenderSetSplash(void);
int			DGRenderTestColor(int xPosition, int yPosition);

void		DGRenderClearScene(void);
void		DGRenderCopyScene(void);
void		DGRenderSaveScene(void);
void		DGRenderUpdateScene(void);

extern const unsigned char _binary_def_particle_start[];
extern const unsigned char _binary_def_splash_start[];

#endif // DG_RENDER_H
