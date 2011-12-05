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


enum DG_ACTION_TYPES {
	DG_ACTION_CUSTOM,
	DG_ACTION_FEEDBACK,
	DG_ACTION_SWITCH
};

typedef struct {
	unsigned type;
	int custom_handler;
	char feedback[DG_MAX_FEEDBACK];
	char feedback_audio[DG_MAX_OBJNAME];
	DGObject* target;
} DGAction;


// New / Release


// TODO: Add auto = false
// TODO: Add structure for retrieving coordinates

DGObject*		DGSpotNew(short* withArrayOfCoordinates, unsigned sizeOfArray, unsigned onFace, int withFlags);
void			DGSpotRelease(DGObject* spot);

// State Changes

void			DGSpotDisable(DGObject* spot);
void			DGSpotEnable(DGObject* spot);
void			DGSpotPlay(DGObject* spot);
void			DGSpotStop(DGObject* spot);
void			DGSpotToggle(DGObject* spot);

// Checks

DGBool			DGSpotHasAction(DGObject* spot);
DGBool			DGSpotHasAudio(DGObject* spot);
DGBool			DGSpotHasColor(DGObject* spot);
DGBool			DGSpotHasImage(DGObject* spot);
DGBool			DGSpotHasTexture(DGObject* spot);
DGBool			DGSpotHasVideo(DGObject* spot);
DGBool			DGSpotIsEnabled(DGObject* spot);
DGBool			DGSpotIsPlaying(DGObject* spot);

// Gets

DGAction*		DGSpotAction(DGObject* spot);
DGObject*       DGSpotAudio(DGObject* spot);
int				DGSpotColor(DGObject* spot);
short*			DGSpotArrayOfCoordinates(DGObject* spot);
unsigned		DGSpotSizeOfArray(DGObject* spot);
unsigned		DGSpotFace(DGObject* spot);
const char*		DGSpotImage(DGObject* spot);
DGObject*		DGSpotTexture(DGObject* spot);
DGObject*		DGSpotVideo(DGObject* spot);

// Sets

void			DGSpotResize(DGObject* spot, short* withNewArrayOfCoordinates, unsigned sizeOfNewArray);
void			DGSpotSetAction(DGObject* spot, DGAction* anAction);
void			DGSpotSetAudio(DGObject* spot, DGObject* anAudio);
void			DGSpotSetColor(DGObject* spot, int aColor);
void			DGSpotSetImage(DGObject* spot, const char* anImageFilename);
void			DGSpotSetTexture(DGObject* spot, DGObject* aTexture);
void			DGSpotSetVideo(DGObject* spot, DGObject* aVideo);

#endif // DG_SPOT_H
