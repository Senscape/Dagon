/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_CAMERA_H
#define	DG_CAMERA_H

#include "DGCommon.h"


// WARNING: Factors change according to architecture!!
// These defaults are based on a 1024x768 resolution and 60 fps. Factors are applied accordingly.

enum DG_CAMERA_DEFAULTS {
	DG_CAM_HPANFACTOR = 7500,
	DG_CAM_VPANFACTOR = 10000, // Higher to smoothen the vertical panning
	DG_CAM_NEUTRALZONE = 200,
	DG_CAM_MAXPANSPEED = 400
};

enum DG_CAMERA_DIRECTIONS {
	DG_CAM_FRONT = 0,
	DG_CAM_RIGHT = 90,
	DG_CAM_BACK = 180,
	DG_CAM_LEFT = 270,
	DG_CAM_TOP = 360,
	DG_CAM_BOTTOM = -360,
	DG_CAM_CURRENT = 0xFFF
};

enum DG_BREATHE_STATES {
	DG_CAM_INSPIRE,
	DG_CAM_EXPIRE,
	DG_CAM_PAUSE
};

enum DG_CAMERA_STATES {
	DG_CAM_IDLE,
	DG_CAM_ACCEL,
	DG_CAM_DEACCEL,
	DG_CAM_LOOKAT,
	DG_CAM_LOOKAT_OVER,
	DG_CAM_PAN
};

typedef struct {
	int x;
	int y;
} DGPoint;

typedef struct {
	int width;
	int height;
} DGSize;

typedef struct {
	DGPoint point;
	DGSize size;
} DGRect;


// Init

void		DGCameraInitialize(int withWidth, int andHeight);

// State Changes

void		DGCameraLoop(void);
void		DGCameraLookAt(int horizontalAngle, int verticalAngle, DGBool isInstantTransition); // In degrees
void		DGCameraPan(int xPosition, int yPosition);
void		DGCameraUpdate(void);
void		DGCameraSimulateWalk(void);

// Checks

int			DGCameraDeltaX(void);
int			DGCameraDeltaY(void);
int			DGCameraState(void);

// Gets

DGBool		DGCameraIsBreathingEnabled(void);
DGBool		DGCameraIsWalkingEnabled(void);

int			DGCameraHorizontalAngle(void);
int			DGCameraVerticalAngle(void);
float		DGCameraFieldOfView(void);
int			DGCameraNeutralZone(void);
DGPoint     DGCameraOrigin(void);
int			DGCameraPanningSpeed(void);
int			DGCameraVerticalLimit(void);
DGSize		DGCameraViewport(void);

// Sets

void		DGCameraEnableBreathing(DGBool flag);
void		DGCameraEnableWalking(DGBool flag);

void		DGCameraSetFieldOfView(float angle);
void		DGCameraSetNeutralZone(int size);
void		DGCameraSetOrigin(int xPosition, int yPosition);
void		DGCameraSetOrthoView(void);
void		DGCameraSetPanningSpeed(int speed);
void		DGCameraSetPerspectiveView(void);
void		DGCameraSetVerticalLimit(int verticalAngle); // In degrees
void		DGCameraSetViewport(int withWidth, int andHeight);

#endif // DG_CAMERA_H
