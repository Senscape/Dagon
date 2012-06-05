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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGCamera.h"
#include "DGConfig.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGCamera::DGCamera() {
    config = &DGConfig::getInstance();

    _accelH = 0.0f;
    _accelV = 0.0f;
    
    _angleH = 0.0f;
    _angleV = 0.0f;
    
    _angleHLimit = (float)M_PI * 2;
    _angleVLimit = (float)M_PI / 2;
    
    _deltaX = 0;
    _deltaY = 0;
    
    _fovCurrent = DGCamFieldOfView;
    _fovNormal = DGCamFieldOfView;

    _dragNeutralZone = DGCamDragNeutralZone;
    _freeNeutralZone = DGCamFreeNeutralZone;
    
    _orientation[0] = 0.0f;
    _orientation[1] = 0.0f;
    _orientation[2] = -1.0f;
    _orientation[3] = 0.0f;
    _orientation[4] = 1.0f; 
    _orientation[5] = 0.0f; 
    
    _position[0] = 0.0f;
    _position[1] = 0.0f;
    _position[2] = 0.0f;
    
    _breatheDirection = DGCamPause;
    _breatheFactor = 0.0f;
    _cameraDisplace = 0.0f;
    _inertia = 1 / (float)DGCamInertia;
    _isWalking = false;
    
    _motionDown = 0.0f;
    _motionLeft = 0.0f;
    _motionRight = 0.0f;
    _motionUp = 0.0f;
    
    _maxSpeed = 1.0f / (float)DGCamMaxSpeed;
    _speedH = 0.0f;
    _speedV = 0.0f;    
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGCamera::~DGCamera() {
    // Nothing to do here
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGCamera::beginOrthoView() {
    // Switch to the projection view
    glMatrixMode(GL_PROJECTION);
    
    // Save its current state and load a new identity
    glPushMatrix();
    glLoadIdentity();
    
    // Now we prepare our orthogonal projection
    glOrtho(0, _viewport.width, _viewport.height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    
    // Note that transformations have been applied to the GL_MODELVIEW
    // so we must reload the identity matrix
    glLoadIdentity();
}

void DGCamera::endOrthoView() {
    // Go back to the projection view and its previous state
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    // Leave everything in model view just as it were before
    glMatrixMode(GL_MODELVIEW);
}

float DGCamera::fieldOfView() {
    return _fovCurrent;
}

int DGCamera::neutralZone() {
    switch (config->controlMode) {
        case DGMouseDrag:
            return _dragNeutralZone;
        case DGMouseFixed:
            return 0;            
        case DGMouseFree:
            return _freeNeutralZone;
    }
    
   return 0;
}

float* DGCamera::orientation() {
    return _orientation;
}

void DGCamera::pan(int xPosition, int yPosition) {
    if (xPosition > _panRegion.size.width)
        _deltaX = xPosition - _panRegion.size.width;
    else if (xPosition < _panRegion.origin.x)
        _deltaX = xPosition - _panRegion.origin.x;
    else _deltaX = 0.0f;
        
    if (yPosition > _panRegion.size.height)
        _deltaY = _panRegion.size.height - yPosition;
    else if (yPosition < _panRegion.origin.y)
        _deltaY = _panRegion.origin.y - yPosition;
    else _deltaY = 0.0f;
    
    if (config->controlMode == DGMouseDrag) {
        _speedH = fabs(_deltaX) / (float)(DGCamSpeedFactor * 2); // Make movement even smoother
        _speedV = fabs(_deltaY) / (float)(DGCamSpeedFactor * 2);
    }
    else {
        _speedH = fabs(_deltaX) / (float)DGCamSpeedFactor;
        _speedV = fabs(_deltaY) / (float)DGCamSpeedFactor;
        
    }
}

float* DGCamera::position() {
    return _position;
}

void DGCamera::simulateWalk() {
    _fovCurrent = _fovNormal + (float)DGCamWalkIntensity;
    _isWalking = true;
}

void DGCamera::setFieldOfView(float fov) {
    _fovNormal = DGCamFieldOfView;
}

void DGCamera::setNeutralZone(int zone) {
    switch (config->controlMode) {
        case DGMouseDrag:
            _dragNeutralZone = zone;
            break;
            
        case DGMouseFixed:
            // Nothing to do here
            break;
            
        case DGMouseFree:
            // We calculate the factor to stretch the neutral zone with
            // the display height
            float factor = (float)_viewport.height / (float)DGDefDisplayHeight;
            
            _freeNeutralZone = zone;
            
            // Update panning regions with the new neutral zone
            _panRegion.origin.x = (_viewport.width / 2) - _freeNeutralZone * factor;
            _panRegion.origin.y = (_viewport.height / 2) - _freeNeutralZone * factor;
            _panRegion.size.width = (_viewport.width / 2) + _freeNeutralZone * factor;
            _panRegion.size.height = (_viewport.height / 2) + _freeNeutralZone * factor;
            
            break;
    }
}

void DGCamera::setViewport(int width, int height) {
    // NOTE: if screen size is changed while in Drag mode, the neutral zone for Free mode isn't updated
    
    _viewport.width = width;
    _viewport.height = height;
    
    // This forces the new display factor to be applied to the
    // current neutral zone (only in Free mode)
    if (config->controlMode == DGMouseFree)
        setNeutralZone(_freeNeutralZone);
    
    glViewport(0, 0, (GLint)_viewport.width, (GLint)_viewport.height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // We need a very close clipping point because the cube is rendered in a small area
    gluPerspective(_fovCurrent, (GLfloat)_viewport.width / (GLfloat)_viewport.height, 0.1f, 10.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void DGCamera::startDragging(int xPosition, int yPosition) {
    _panRegion.origin.x = xPosition - _dragNeutralZone;
    _panRegion.origin.y = yPosition - _dragNeutralZone;
    _panRegion.size.width = xPosition + _dragNeutralZone;
    _panRegion.size.height = yPosition + _dragNeutralZone;
}

void DGCamera::stopDragging() {
    _panRegion.origin.x = (_viewport.width / 2) - _dragNeutralZone;
    _panRegion.origin.y = (_viewport.height / 2) - _dragNeutralZone;
    _panRegion.size.width = (_viewport.width / 2) + _dragNeutralZone;
    _panRegion.size.height = (_viewport.height / 2) + _dragNeutralZone;
    
    this->pan(config->displayWidth / 2, config->displayHeight / 2);
}

void DGCamera::update() {
    if (_isWalking) {
        // Perform walk FX operations
        if (_fovCurrent > _fovNormal)
            _fovCurrent -= ((float)10 / DGCamWalkIntensity) * config->globalSpeed();
        else {
            _fovCurrent = _fovNormal;
            _isWalking = false;
        }
        
        this->setViewport(config->displayWidth, config->displayHeight);
    }
    
    // Calculate horizontal motion
    if (_deltaX > 0) {
        if (_motionRight < _maxSpeed)
            _motionRight += _speedH;
        
        // Apply inertia to opposite direction
        if (_motionLeft > 0.0f)
            _motionLeft -= _inertia;        
    }
	else if (_deltaX < 0) {
        if (_motionLeft < _maxSpeed)
            _motionLeft += _speedH;
        
        // Inertia        
        if (_motionRight > 0.0f)
            _motionRight -= _inertia;        
    }
    else {
        // Decrease leftward motion
        if (_motionLeft > 0.0f)
            _motionLeft -= _inertia;
        else
            _motionLeft = 0.0f;
        
        // Decrease rightward motion
        if (_motionRight > 0.0f)
            _motionRight -= _inertia;
        else
            _motionRight = 0.0f;
    }
    
    // Calculate vertical motion
    if (_deltaY > 0) {  
        if (_motionDown < _maxSpeed)
            _motionDown += _speedV;
        
        // Apply inertia to opposite direction
        if (_motionUp > 0.0f)
            _motionUp -= _inertia;        
    }
	else if (_deltaY < 0) {
        if (_motionUp < _maxSpeed)
            _motionUp += _speedV;
        
        // Inertia
        if (_motionDown > 0.0f)
            _motionDown -= _inertia;        
    }
    else {
        // Decrease downward motion
        if (_motionDown > 0.0f)
            _motionDown -= _inertia;
        else
            _motionDown = 0.0f;
        
        // Decrease upward motion
        if (_motionUp > 0.0f)
            _motionUp -= _inertia;
        else
            _motionUp = 0.0f;
    }
    
    // Calculate horizontal acceleration
    if (_deltaX) {
        if (_accelH < 1.0f)
            _accelH += 1.0f / (float)DGCamAccelerationFactor;
    }
    else {
        // Deaccelerate        
        if (_accelH > 0.0f)
            _accelH -= 1.0f / (float)DGCamAccelerationFactor;
        else
            _accelH = 0.0f;
    }
    
    // Calculate vertical acceleration
    if (_deltaY) {
        if (_accelV < 1.0f)
            _accelV += 1.0f / ((float)DGCamAccelerationFactor / 4);
    }
    else {
        // Deaccelerate
        if (_accelV > 0.0f)
            _accelV -= 1.0f / ((float)DGCamAccelerationFactor / 4);
        else
            _accelV = 0.0f;
    }
    
    // Apply horizontal motion
    if (_motionLeft)
        _angleH -= sin(_accelH) * _motionLeft;
    
    if (_motionRight)
        _angleH += sin(_accelH) * _motionRight;
    
    // Apply vertical motion
    if (_motionDown)
        _angleV += sin(_accelV) * _motionDown;
    
    if (_motionUp)
        _angleV -= sin(_accelV) * _motionUp;    
    
    // Limit horizontal rotation
    if (_angleH > (GLfloat)_angleHLimit)
        _angleH = 0.0f;
    else if (_angleH < 0.0f)
        _angleH = (GLfloat)_angleHLimit;
    
    // Limit vertical rotation
    if (_angleV > (GLfloat)_angleVLimit)
        _angleV = (GLfloat)_angleVLimit;
    else if (_angleV < -(GLfloat)_angleVLimit)
        _angleV = -(GLfloat)_angleVLimit;
    
    _orientation[0] = (float)sin(_angleH);
    _orientation[1] = _angleV;
    _orientation[2] = (float)-cos(_angleH);
    
    static float j = 0.0f;
    static float breatheInspireLimit = 0.0f;
    static float breatheExpireLimit = 0.0f;    
    static float breatheWait = 0.0f;
    
    switch (_breatheDirection) {
        case DGCamPause:
            j += 0.1f;
            if (j >= breatheWait) {
                j = 0.0f;
                breatheWait = 0.0f;
                breatheInspireLimit = ((float)((rand() % 3) + 5)) / 10.0f;
                _breatheDirection = DGCamInspire;
            }
            break;
        case DGCamInspire:
            if (_breatheFactor < breatheInspireLimit) _breatheFactor += (breatheInspireLimit / 100.0f);
            else {
                breatheExpireLimit = ((float)((rand() % 2) + 6)) / 10.0f;
                _breatheDirection = DGCamExpire;
            }
            
            _cameraDisplace = sin(_breatheFactor) * cos(_breatheFactor) / 20;
            break;
        case DGCamExpire:
            if (_breatheFactor > -breatheExpireLimit) _breatheFactor -= (breatheExpireLimit / 100.0f);
            else {
                _breatheDirection = DGCamPause;
                breatheWait = (float)((rand() % 3) + 1); // Make the breathing more irregular
            }
            
            _cameraDisplace = sin(_breatheFactor) * cos(_breatheFactor) / 20; 
            break;    
    }
    
    gluLookAt(_position[0], _position[1] + (_cameraDisplace / 4), _position[2],
              _orientation[0], _orientation[1] + _cameraDisplace, _orientation[2],
              _orientation[3], _orientation[4], _orientation[5]);
}
