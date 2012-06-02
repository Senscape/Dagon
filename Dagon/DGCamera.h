////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
//
////////////////////////////////////////////////////////////

#ifndef DG_CAMERA_H
#define DG_CAMERA_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum DGCameraDefaults {
    DGCamAccelerationFactor = 100,
    DGCamDragNeutralZone = 50,    
	DGCamFieldOfView = 55,
    DGCamFreeNeutralZone = 250,    
    DGCamInertia = 750, // Maintain motion after panning stops
    DGCamMaxSpeed = 30,
    DGCamSpeedFactor = 500000 // Make movement smoother, duplicated in Drag mode
};

class DGConfig;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGCamera {
    DGConfig* config;
    
    float _accelH;
    float _accelV;
    
    GLfloat _angleH;
    GLfloat _angleV;

    GLfloat _angleHLimit;
    GLfloat _angleVLimit;
    
    GLfloat _fovCurrent;
    GLfloat _fovNormal;
    
    int _deltaX;
    int _deltaY;
    
    float _orientation[6];
    float _position[3];

    int _dragNeutralZone;
    int _freeNeutralZone;
    
    DGSize _viewport;
    DGRect _panRegion;
    
    float _inertia;
    
    float _motionDown;
    float _motionLeft;
    float _motionRight;
    float _motionUp;    
    
    float _maxSpeed;
    float _speedH;
    float _speedV;    
    
public:
    DGCamera();
    ~DGCamera();
    
    void beginOrthoView();
    void endOrthoView();
    float fieldOfView();
    int neutralZone();
    float* orientation();
    void pan(int xPosition, int yPosition);
    float* position();
    void setFieldOfView(float fov);
    void setNeutralZone(int zone);
    void setViewport(int width, int height);

    // For the Drag mode
    void startDragging(int xPosition, int yPosition); // Recreates the neutral zone
    void stopDragging(); // Resets the neutral zone
    
    void update();
};

#endif // DG_CAMERA_H
