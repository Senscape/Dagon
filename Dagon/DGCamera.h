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

enum DG_Camera_Defaults {
	DG_CAM_HPANFACTOR = 7500,
	DG_CAM_VPANFACTOR = 10000, // Higher to smoothen the vertical panning
	DG_CAM_NEUTRALZONE = 200,
	DG_CAM_MAXPANSPEED = 400
};

enum DG_Camera_Directions {
	DG_CAM_FRONT = 0,
	DG_CAM_RIGHT = 90,
	DG_CAM_BACK = 180,
	DG_CAM_LEFT = 270,
	DG_CAM_TOP = 360,
	DG_CAM_BOTTOM = -360,
	DG_CAM_CURRENT = 0xFFF
};

enum DG_Breathe_States {
	DG_CAM_INSPIRE,
	DG_CAM_EXPIRE,
	DG_CAM_PAUSE
};

enum DG_Camera_States {
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
} DG_POINT;

typedef struct {
	int width;
	int height;
} DG_SIZE;

typedef struct {
	DG_POINT point;
	DG_SIZE size;
} DG_RECT;


// Init

void		dg_camera_init(int width, int height);

// State Changes

void		dg_camera_loop(void);
void		dg_camera_lookat(int angle_h, int angle_v, DG_BOOL instant); // In degrees
void		dg_camera_pan(int x, int y);
void		dg_camera_update(void);
void		dg_camera_walk(void);

// Checks

int			dg_camera_delta_x(void);
int			dg_camera_delta_y(void);
int			dg_camera_state(void);

// Gets

int			dg_camera_angle_h(void);
int			dg_camera_angle_v(void);
DG_BOOL		dg_camera_breathe_fx(void);
float		dg_camera_fov(void);
int			dg_camera_neutral_zone(void);
DG_POINT	dg_camera_origin(void);
int			dg_camera_pan_speed(void);
int			dg_camera_vertical_limit(void);
DG_SIZE		dg_camera_viewport(void);
DG_BOOL		dg_camera_walk_fx(void);

// Sets

void		dg_camera_set_breathe_fx(DG_BOOL flag);
void		dg_camera_set_fov(float value);
void		dg_camera_set_neutral_zone(int value);
void		dg_camera_set_origin(int x, int y);
void		dg_camera_set_ortho(void);
void		dg_camera_set_pan_speed(int value);
void		dg_camera_set_perspective(void);
void		dg_camera_set_vertical_limit(int value); // In degrees
void		dg_camera_set_viewport(int width, int height);
void		dg_camera_set_walk_fx(DG_BOOL flag);

#endif // DG_CAMERA_H
