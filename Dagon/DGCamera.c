/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGCamera.h"
#include "DGCommonGL.h"
#include "DGConfig.h"
#include "DGLog.h"

#define	DG_CAM_DIRLEFT	-1
#define	DG_CAM_DIRRIGHT	1
#define	DG_CAM_DIRUP	1
#define	DG_CAM_DIRDOWN	-1


////////////////////////////////////////////////////////////////////////////////
///// Private Data														   /////
////////////////////////////////////////////////////////////////////////////////

struct dg_camera_data {
	GLfloat h_angle;
    GLfloat v_angle;
	GLfloat h_angle_target; // for lookat
    GLfloat v_angle_target; // for lookat
	float h_angle_limit;
	float v_angle_limit;
	
	GLint view_width;
	GLint view_height;
	float view_width_factor;
	float view_height_factor;
	
	int pan_speed;
	int	pan_origin_x;
	int	pan_origin_y;
	int	pan_coord_x;
	int	pan_coord_y;
	int pan_region_left;
	int pan_region_right;
	int pan_region_up;
	int pan_region_down;
	int state;
	
	DGBool breathe_enabled;
	float breathe_factor;
	float breathe_pos;
	int breathe_state;
	
	DGBool walk_enabled;
	DGBool is_walking;
	float walk_time;
	
	int neutral_zone;
	int curr_neutral_zone;
	GLfloat norm_fov;
	GLfloat curr_fov;
	
	int delta_x;
	int delta_y;
	float accel;
	float tilt;
} data;

////////////////////////////////////////////////////////////////////////////////
///// Private Prototypes												   /////
////////////////////////////////////////////////////////////////////////////////

void _breathe(void);
void _update_pan_regions(void);

////////////////////////////////////////////////////////////////////////////////
///// Init																   /////
////////////////////////////////////////////////////////////////////////////////

void DGCameraInitialize(int withWidth, int andHeight) {
	DGLogTrace(DG_MOD_CAMERA, "Initializing camera manager...");
	DGLogInfo(DG_MOD_CAMERA, "Viewport size: %dx%d", withWidth, andHeight);
	
	data.pan_speed = DG_CAM_MAXPANSPEED;
	data.h_angle = 0.0f;
	data.v_angle = 0.0f;
	data.h_angle_target = 0.0f;
	data.v_angle_target = 0.0f;
	data.neutral_zone = DG_CAM_NEUTRALZONE;
	data.curr_neutral_zone = DG_CAM_NEUTRALZONE;
	_update_pan_regions();
	
	data.norm_fov = 55.0f;
	data.curr_fov = 55.0f;
	data.view_width = (GLint)withWidth;
	data.view_height = (GLint)andHeight;
	data.h_angle_limit = (float)M_PI*2;
	data.v_angle_limit = (float)M_PI/2;
	data.pan_origin_x = (int)data.view_width / 2;
	data.pan_origin_y = (int)data.view_height / 2;
	data.state = DG_CAM_IDLE;
	
	data.breathe_enabled = DG_YES;
	data.breathe_factor = 0.0f;
	data.breathe_pos = 0.0f;
	data.breathe_state = DG_CAM_INSPIRE;

	data.walk_enabled = DG_YES;
	data.is_walking = DG_NO;
	data.walk_time = 0.0f;
	
	data.delta_x = 0;
	data.delta_y = 0;
	data.accel = 0.0f;
	data.tilt = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - State Changes									   /////
////////////////////////////////////////////////////////////////////////////////

int	DGCameraHorizontalAngle() {
	return (int)((data.h_angle * 360.0f) / data.h_angle_limit);
}

int	DGCameraVerticalAngle() {
	return (int)((data.v_angle * 360.0f) / data.v_angle_limit);
}

void DGCameraLookAt(int horizontalAngle, int verticalAngle, DGBool isInstantTransition) {
	if (isInstantTransition) {
		if (horizontalAngle != DG_CAM_CURRENT)
			data.h_angle = (horizontalAngle * data.h_angle_limit) / 360.0f;

		if (verticalAngle != DG_CAM_CURRENT)
			data.v_angle = (verticalAngle * data.v_angle_limit) / 360.0f;
	}
	else {
		data.state = DG_CAM_LOOKAT;
		
		if (horizontalAngle != DG_CAM_CURRENT)
			data.h_angle_target = (horizontalAngle * data.h_angle_limit) / 360.0f;
		else
			data.h_angle_target = data.h_angle;
			
		if (verticalAngle != DG_CAM_CURRENT)
			data.v_angle_target = (verticalAngle * data.v_angle_limit) / 360.0f;
		else
			data.v_angle_target = data.v_angle;
	}
}

void DGCameraLoop() {
	static short h_dir;
	static short v_dir;
	
	if (data.state == DG_CAM_LOOKAT_OVER) // State is reset when the loop restarts
		data.state = DG_CAM_IDLE;
	
	if (data.state == DG_CAM_LOOKAT) {
		// Put all of this in another struct...
		static DGBool h_done = DG_NO;
		static DGBool v_done = DG_NO;
		static float lookat_accel = 0.0f;
		static float lookat_speed = 0.02f;
		float error = 0.02f;
		
        // Failsafe
        if (!data.is_walking && !DGConfig.scare) {
            // FIXME: lookat overrides but this whole loop is sucky... revise!
            
            // The speed in each direction should be proportional to the distances.
            // Movement in h and v should stop "at the same time".
            
            // If the h angle is 0 (front), then we must consider this special case:
            
            // Error should be no higher than 0.01 and speed should decrease as target approaches.
            
            if (data.h_angle_target == 0) {
                if (data.h_angle > M_PI)
                    data.h_angle_target = (float)(M_PI*2);
            }
            
            if (!h_done) {
                if (data.h_angle < data.h_angle_target)
                    data.h_angle += lookat_speed * DGConfig.global_speed * lookat_accel;
                else if (data.h_angle > data.h_angle_target)
                    data.h_angle -= lookat_speed * DGConfig.global_speed * lookat_accel;
            }
            
            if (!v_done) {
                if (data.v_angle < data.v_angle_target)
                    data.v_angle += lookat_speed/2 * DGConfig.global_speed * lookat_accel;
                else if (data.v_angle > data.v_angle_target)
                    data.v_angle -= lookat_speed/2 * DGConfig.global_speed * lookat_accel;
            }
            
            lookat_accel += 0.02f;
            
            // Fix to avoid vertical accumulation
            if (data.breathe_pos > 0.0f)
                data.breathe_pos -= lookat_speed/2 * DGConfig.global_speed * lookat_accel;
            if (data.breathe_pos < 0.0f)
                data.breathe_pos += lookat_speed/2 * DGConfig.global_speed * lookat_accel;
            
            if ((data.h_angle > (data.h_angle_target - error)) && (data.h_angle < (data.h_angle_target + error)))
                h_done = DG_YES;
                
            // Error check doesn't work well here
            if ((data.v_angle > (data.v_angle_target - error)) && (data.v_angle < (data.v_angle_target + error)))
                v_done = DG_YES;
                
            // Try doing the is_walking check here
            if (h_done && v_done) {
                h_done = DG_NO;
                v_done = DG_NO;
                lookat_accel = 0.0f;
                data.state = DG_CAM_LOOKAT_OVER;
            }
            
            return;
        }
	}
	
	if (data.pan_coord_x > data.pan_region_right) {
		data.delta_x = data.pan_coord_x - data.pan_region_right;
		h_dir = DG_CAM_DIRRIGHT;
	}
	else if (data.pan_coord_x < data.pan_region_left) {
		data.delta_x = data.pan_coord_x - data.pan_region_left;
		h_dir = DG_CAM_DIRLEFT;
	}
	else {
		data.delta_x = 0;
	}
	
	if (data.pan_coord_y > data.pan_region_up) {
		data.delta_y = data.pan_coord_y - data.pan_region_up;
		v_dir = DG_CAM_DIRUP;
	}
	else if (data.pan_coord_y < data.pan_region_down) {
		data.delta_y = data.pan_coord_y - data.pan_region_down;
		v_dir = DG_CAM_DIRDOWN;
	}
	else {
		data.delta_y = 0;
	}
	
	if (data.delta_x || data.delta_y) {
		switch (data.state) {
			case DG_CAM_DEACCEL:
			case DG_CAM_IDLE:
			case DG_CAM_LOOKAT_OVER:
				data.state = DG_CAM_ACCEL;
				data.accel = 0.0f;
				break;
			case DG_CAM_PAN:
				break;
			case DG_CAM_ACCEL:
				if (data.accel < 1.0f)
					data.accel += 0.01f * DGConfig.global_speed;
				else {
					data.state = DG_CAM_PAN;
					data.accel = 1.0f;
				}
				
				/*if (data.tilt < 0.1f)
					data.tilt += 0.02f * config.global_speed * h_dir;*/
				break;
		}
		
		if (data.delta_x) {
			// Limit panning speed while keeping direction
			if (abs(data.delta_x) > data.pan_speed)
				data.delta_x = data.pan_speed * h_dir;
			
			data.h_angle += ((float)data.delta_x / (float)DG_CAM_HPANFACTOR) * data.accel * DGConfig.global_speed;
			
			// Limit rotation
			if (data.h_angle > (GLfloat)data.h_angle_limit)
				data.h_angle = 0.0f;
			else if (data.h_angle < 0.0f)
				data.h_angle = (GLfloat)data.h_angle_limit;
		}
		
		if (data.delta_y) {
			// Limit panning speed while keeping direction
			if (abs(data.delta_y) > data.pan_speed)
				data.delta_y = data.pan_speed * v_dir;
			
			data.v_angle += ((float)data.delta_y / (float)DG_CAM_VPANFACTOR) * data.accel * DGConfig.global_speed;
			
			// Limit rotation
			if (data.v_angle > (GLfloat)data.v_angle_limit)
				data.v_angle = (GLfloat)data.v_angle_limit;
			else if (data.v_angle < -(GLfloat)data.v_angle_limit)
				data.v_angle = -(GLfloat)data.v_angle_limit;
		}
	}
	else {
		switch (data.state) {
			case DG_CAM_ACCEL:
			case DG_CAM_PAN:
				data.state = DG_CAM_DEACCEL;
				break;
			case DG_CAM_DEACCEL:
			case DG_CAM_IDLE:	
				if (data.accel > 0.0f) {
					data.accel -= 0.01f * DGConfig.global_speed;
					data.h_angle += data.accel * (2 / (float)DG_CAM_HPANFACTOR) * h_dir;
					data.v_angle += data.accel * (2 / (float)DG_CAM_VPANFACTOR) * v_dir;
				}
				else {
					data.state = DG_CAM_IDLE;
					data.accel = 0.0f;
				}
				/*if (data.tilt > 0.0f)
					data.tilt -= 0.04f * config.global_speed;
				else if (data.tilt < 0.0f)
					data.tilt += 0.04f * config.global_speed;
				break;*/
		}
	}
	
	_breathe();
}

void DGCameraPan(int xPosition, int yPosition) {
	data.pan_coord_x = xPosition;
	data.pan_coord_y = yPosition;
}

void DGCameraUpdate() {
	float lx;
    float lz;
	
	lx = (float)sin(data.h_angle);
    lz = (float)-cos(data.h_angle);
	
    if (DGConfig.scare)
        gluLookAt(0.0f, data.breathe_pos/4, 0.0f,
                  lx, data.v_angle + data.breathe_pos, lz,
                  0.0f, 1.0f, 0.0f);
    else
        gluLookAt(0.0f, data.breathe_pos/4, 0.0f,
                  lx, data.v_angle + data.breathe_pos, lz,
                  0.0f, 1.0f, 0.0f);
}

void DGCameraSimulateWalk() {
	if (!data.is_walking) 
		data.is_walking = DG_YES;
	else
		data.walk_time = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Checks											   /////
////////////////////////////////////////////////////////////////////////////////

int	DGCameraDeltaX() {
	// This is momentum rather than delta...
	return (int)(data.delta_x * data.accel);
}

int	DGCameraDeltaY() {
	// This is momentum rather than delta...
	if (abs((int)data.v_angle) < abs((int)data.v_angle_limit))
		return (int)(data.delta_y * data.accel);
	else
		return 0;
}

int	DGCameraState() {
	return data.state;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Gets											       /////
////////////////////////////////////////////////////////////////////////////////

DGBool	DGCameraIsBreathingEnabled() {
	return data.breathe_enabled;
}

float DGCameraFieldOfView() {
	return data.norm_fov;
}

int	DGCameraNeutralZone() {
	return data.neutral_zone;
}

DGPoint DGCameraOrigin() {
	static DGPoint point;
	
	point.x = data.pan_origin_x;
	point.y = data.pan_origin_y;
	
	return point;
}

int	DGCameraPanningSpeed() {
	return data.pan_speed;
}

int DGCameraVerticalLimit() {
	return (int)(data.v_angle_limit * 180);
}

DGSize	DGCameraViewport() {
	static DGSize size;
	
	size.width = data.view_width;
	size.height = data.view_height;
	
	return size;
}

DGBool	DGCameraIsWalkingEnabled() {
	return data.walk_enabled;
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation - Sets												   /////
////////////////////////////////////////////////////////////////////////////////

void DGCameraEnableBreathing(DGBool flag) {
	data.breathe_enabled = flag;
}

void DGCameraSetFieldOfView(float angle) {
	data.norm_fov = angle;
	data.curr_fov = angle;
}

void DGCameraSetNeutralZone(int size) {
	data.neutral_zone = size;
	_update_pan_regions();
}

void DGCameraSetOrigin(int xPosition, int yPosition) {
	data.pan_origin_x = xPosition;
	data.pan_origin_y = yPosition;
	_update_pan_regions();
}

void DGCameraSetOrthoView() {
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef DG_PLATFORM_IOS
    glOrthof(0, data.view_width, 0, data.view_height, -1, 1);
#else
	glOrtho(0, data.view_width, 0, data.view_height, -1, 1);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void DGCameraSetPanningSpeed(int speed) {
	data.pan_speed = speed;
}

void DGCameraSetPerspectiveView() {
	glViewport(0, 0, data.view_width, data.view_height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void DGCameraSetVerticalLimit(int verticalAngle) {
	data.v_angle_limit = verticalAngle / (float)180.0f;
}

void DGCameraSetViewport(int withWidth, int andHeight) {
	data.view_width = (GLint)withWidth;
	data.view_height = (GLint)andHeight;
	
	data.view_width_factor = (float)data.view_width / (float)DG_DEF_DISPLAYWIDTH;
	data.view_height_factor = (float)data.view_height / (float)DG_DEF_DISPLAYHEIGHT;
	
	// TODO: It should be possible to configure the neutral zone separately
	data.curr_neutral_zone = (int)(data.neutral_zone * data.view_width_factor); // Neutral zone is square, so we base the factor on the width
	
	glViewport(0, 0, data.view_width, data.view_height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// We need a very close clipping point because the cube is rendered in a small area
	gluPerspective(data.curr_fov, (GLfloat)data.view_width / (GLfloat)data.view_height, 0.1f, 10.0f);
	
	// TODO: This should DG_NOT be enforced on mobile
	// Reset the point origin upon viewport change
	data.pan_origin_x = (int)data.view_width / 2;
	data.pan_origin_y = (int)data.view_height / 2;
	_update_pan_regions();
}

void DGCameraEnableWalking(DGBool flag) {
	data.walk_enabled = flag;
}

////////////////////////////////////////////////////////////////////////////////
///// Private Functions													   /////
////////////////////////////////////////////////////////////////////////////////

void _breathe() {
	static float j = 0.0f;
	static float breathe_wait = 0.0f;
	static float limit_factor = 1.0f;
	static float walk_factor = 1.0f;
    
    // HACK -- enable for heavy breathing
    // Close but both are not compatible -- height is gradually increased
    if (DGConfig.scare) {
        if (data.breathe_state == DG_CAM_PAUSE)
            data.breathe_state = DG_CAM_INSPIRE;
        
        limit_factor = 15.0f;
        walk_factor = 50.0f * DGConfig.global_speed;
    }
	
	if (data.walk_enabled) {
		// TODO: Lots of hardcodes here. Also, revise the math.
		if (data.is_walking) {
			float aperture = data.norm_fov + 30.0f;
			
			if (data.breathe_state == DG_CAM_PAUSE)
				data.breathe_state = DG_CAM_INSPIRE;
			
            limit_factor = 10.0f;
			walk_factor = 50.0f * DGConfig.global_speed;
			
            data.curr_fov = aperture - (data.walk_time * (aperture - data.norm_fov));   // Disable in heavy breathing
			data.walk_time += 0.01f * DGConfig.global_speed;
			
			if (data.walk_time > 1.0f) {
				data.curr_fov = data.norm_fov;
				data.walk_time = 0.0f;
				
				data.is_walking = DG_NO;
			}
		}
		else {
            if (!DGConfig.scare) {
                limit_factor = 1.0f;
                walk_factor = 1.0f;
                
                // Correct the breathing factor and position with the last "step"
                if (data.breathe_factor > 0.085f)
                    data.breathe_factor -= 0.1f * DGConfig.global_speed;
                else if (data.breathe_factor < -0.075f)
                    data.breathe_factor += 0.1f * DGConfig.global_speed;
            }
		}
	}
	
	if (data.breathe_enabled) {
		switch (data.breathe_state) {
			case DG_CAM_INSPIRE:
				data.breathe_factor += 0.0018f * walk_factor * DGConfig.global_speed;
				data.breathe_pos += (float)(cos(data.breathe_factor) * (data.breathe_factor / 250.0f));
				if (data.breathe_factor >= 0.075f * limit_factor) data.breathe_state = DG_CAM_EXPIRE;
				break;
			case DG_CAM_EXPIRE:
				data.breathe_factor -= 0.0008f * walk_factor * DGConfig.global_speed;
				data.breathe_pos += (float)(cos(data.breathe_factor) * (data.breathe_factor / 250.0f));
				if (data.breathe_factor <= -0.065f * limit_factor) {
					j = 0.0f;
					
#ifdef DG_PLATFORM_MAC	
					breathe_wait = (float)((arc4random() % 10) + 5);
#else
					breathe_wait = (float)((rand() % 10) + 5);
#endif
					data.breathe_state = DG_CAM_PAUSE;
				}
				break;
			case DG_CAM_PAUSE:
				data.breathe_factor += 0.0001f * DGConfig.global_speed;
				data.breathe_pos += (float)(cos(data.breathe_factor) * (data.breathe_factor / 500.0f));
				j += 0.1f;
				if (j >= breathe_wait)
					data.breathe_state = DG_CAM_INSPIRE;
				break;
		}
	}
}

void _update_pan_regions() {
	data.pan_region_left = data.pan_origin_x - data.curr_neutral_zone;
	data.pan_region_right = data.pan_origin_x + data.curr_neutral_zone;
	data.pan_region_up = data.pan_origin_y + data.curr_neutral_zone;
	data.pan_region_down = data.pan_origin_y - data.curr_neutral_zone;	
}
