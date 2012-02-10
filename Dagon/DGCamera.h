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
#include "DGConfig.h"
#include "DGGeometry.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define DGCamFieldOfView 55
#define DGCamNeutralZone 250

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGCamera {
    DGConfig* config;
    
    GLfloat _angleH;
    GLfloat _angleV;

    GLfloat _angleHLimit;
    GLfloat _angleVLimit;
    
    GLfloat _fovCurrent;
    GLfloat _fovNormal;
    
    int _deltaX;
    int _deltaY;
    
    int _neutralZone;
    
    DGSize _viewport;
    DGRect _panRegion;
    
    int _panDirH;
    int _panDirV;
    
public:
    DGCamera();
    ~DGCamera();
    
    void beginOrthoView();
    void endOrthoView();
    float fieldOfView();
    int neutralZone();
    void pan(int xPosition, int yPosition);
    void setFieldOfView(float fov);
    void setNeutralZone(int zone);
    void setViewport(int width, int height);
    void update();
};

#endif // DG_CAMERA_H
