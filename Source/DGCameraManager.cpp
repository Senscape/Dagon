////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2013 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGCameraManager.h"
#include "Config.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGCameraManager::DGCameraManager() :
    config(Config::instance())
{
    _isInitialized = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGCameraManager::~DGCameraManager() {
    // Nothing to do here
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool DGCameraManager::canBreathe() {
    return _canBreathe;
}

bool DGCameraManager::canWalk() {
    return _canWalk;
}

bool DGCameraManager::isPanning() {
    return _isPanning;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

int DGCameraManager::angleHorizontal() {
    return _toDegrees(_angleH, _angleHLimit);
}

int DGCameraManager::angleVertical() {
    return _toDegrees(_angleV, _angleVLimit);
}

int DGCameraManager::cursorWhenPanning() {
    if (_motionLeft > 0.0f) {
        if (_motionUp > 0.0f) return kCursorDownLeft;
        else if (_motionDown > 0.0f) return kCursorUpLeft;
        
        return kCursorLeft;
    }
    else if (_motionRight > 0.0f) {
        if (_motionUp > 0.0f) return kCursorDownRight;
        else if (_motionDown > 0.0f) return kCursorUpRight;
        
        return kCursorRight;
    }
    else if (_motionUp > 0.0f) return kCursorDown;
    else if (_motionDown > 0.0f) return kCursorUp;
    
    if (config.controlMode == kControlDrag)
        return kCursorDrag;
    else
        return kCursorNormal;
}

float DGCameraManager::fieldOfView() {
    return _fovCurrent;
}

int DGCameraManager::inertia() {
    return (1 / _inertia);
}

int DGCameraManager::maxSpeed() {
    int speed = 1 / _maxSpeed;
    return (100 - speed);
}

float DGCameraManager::motionHorizontal() {
    return (_motionLeft + _motionRight) * _accelH;
}

float DGCameraManager::motionVertical() {
    return (_motionDown + _motionUp) * _accelV;
}

int DGCameraManager::neutralZone() {
    switch (config.controlMode) {
        case kControlDrag:
            return _dragNeutralZone;
        case kControlFixed:
            return 0;
        case kControlFree:
            return _freeNeutralZone;
    }
    
    return 0;
}

float* DGCameraManager::orientation() {
    return _orientation;
}

float* DGCameraManager::position() {
    return _position;
}

int DGCameraManager::speedFactor() {
    return _speedFactor / 10000;
}

int DGCameraManager::verticalLimit() {
    return _toDegrees(_angleVLimit, M_PI * 2);
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void DGCameraManager::setAngleHorizontal(float horizontal) {
    if (horizontal != kCurrent) {
        // Extrapolate the coordinates
        _angleH = _toRadians(horizontal, _angleHLimit);
    }
}

void DGCameraManager::setAngleVertical(float vertical) {
    if (vertical != kCurrent) {
        // Extrapolate the coordinates
        _angleV = _toRadians(vertical, _angleVLimit);
    }
}

void DGCameraManager::setBreathe(bool enabled) {
    _canBreathe = true;
    
    if (enabled) {
        _bob.state = DGCamBreathing;
    }
    else {
        _bob.state = DGCamIdle;
    }
}

void DGCameraManager::setFieldOfView(float fov) {
    if (!_isLocked) {
        _fovNormal = fov;
        _fovCurrent = _fovNormal;
        this->setViewport(config.displayWidth, config.displayHeight); // Update the viewport
    }
}

void DGCameraManager::setInertia(int inertia) {
    _inertia = 1 / (float)inertia;
}

void DGCameraManager::setMaxSpeed(int speed) {
    int maxSpeed = 100 - speed;
    _maxSpeed = 1.0f / (float)maxSpeed;
}

void DGCameraManager::setNeutralZone(int zone) {
    switch (config.controlMode) {
        case kControlDrag:
            _dragNeutralZone = zone;
            break;
            
        case kControlFixed:
        case kControlFree:
            _freeNeutralZone = zone;
            
            int percentageX = ((config.displayWidth * zone) / 100) / 2;
            int percentageY = ((config.displayHeight * zone) / 100) / 2;
            
            // Update panning regions with the new neutral zone
            _panRegion.origin.x = (_viewport.width / 2) - percentageX;
            _panRegion.origin.y = (_viewport.height / 2) - percentageY;
            _panRegion.size.width = (_viewport.width / 2) + percentageX;
            _panRegion.size.height = (_viewport.height / 2) + percentageY;
            
            break;
    }
}

void DGCameraManager::setSpeedFactor(int speed) {
    _speedFactor = speed * 10000;
}

void DGCameraManager::setTargetAngle(float horizontal, float vertical) {
    if (horizontal != kCurrent) {
        // Substract n times pi (fix probable double turn)
        float pi, pos;
        pi = _angleH / (M_PI * 2);
		pos = pi - (float)(floor (pi));
		_angleH = pos * 2 * M_PI;
        
        // Extrapolate the coordinates
        _angleHTarget = _toRadians(horizontal, _angleHLimit);
        
        // Treat special case when angle is NORTH
        // TODO: Certain cases aren't supported in this code; ie:
        // slightly rotated left of North, then setting target angle
        // to East. Needs improving.
        if (_angleHTarget == 0.0f) {
            if (_angleH > M_PI)
                _angleHTarget = (float)(M_PI*2);
        }
        
        // FIXME: This division is related to the amount of configured inertia. Not right.
        _targetHError = fabs(_angleHTarget - _angleH) / 5.0f;
        
        if (_targetHError < 0.02f)
            _targetHError = 0.02f;
    }
    else {
        _angleHTarget = _angleH;
    }
    
    if (vertical != kCurrent) {
        // Extrapolate the coordinates
        _angleVTarget = _toRadians(vertical, _angleVLimit);;
        
        // FIXME: This division is related to the amount of configured inertia. Not right.
        _targetVError = fabs(_angleVTarget - _angleV) / 5.0f;
        
        //if (_targetVError < 0.001f)
        _targetVError = 0.01f;
    }
    else {
        _angleVTarget = _angleV;
    }
}

void DGCameraManager::setVerticalLimit(float limit) {
    _angleVLimit = _toRadians(limit, M_PI * 2);
}

void DGCameraManager::setViewport(int width, int height) {
    // NOTE: if screen size is changed while in Drag mode, the neutral zone for Free mode isn't updated

    _viewport.width = width;
    _viewport.height = height;
    
    // This forces the new display factor to be applied to the
    // current neutral zone (only in Free mode)
    switch (config.controlMode) {
        case kControlDrag:
            this->setNeutralZone(_dragNeutralZone);
        case kControlFixed:
            this->stopDragging(); // Refreshes the neutral zone and centers the cursor
            break;
        case kControlFree:
            setNeutralZone(_freeNeutralZone);
            break;
    }
    
    if (_isInitialized) {
        glViewport(0, 0, (GLint)_viewport.width, (GLint)_viewport.height);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        // We need a very close clipping point because the cube is rendered in a small area
        gluPerspective(_fovCurrent, (GLfloat)_viewport.width / (GLfloat)_viewport.height, 0.1f, 10.0f);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
}

void DGCameraManager::setWalk(bool enabled) {
    _canWalk = enabled;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void DGCameraManager::beginOrthoView() {
    if (!_inOrthoView && _isInitialized) {
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
        
        _inOrthoView = true;
    }
}

void DGCameraManager::endOrthoView() {
    if (_inOrthoView && _isInitialized) {
        // Go back to the projection view and its previous state
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        
        // Leave everything in model view just as it were before
        glMatrixMode(GL_MODELVIEW);
        
        _inOrthoView = false;
    }
}

void DGCameraManager::directPan(int xPosition, int yPosition) {
    static int xPrevious = config.displayWidth / 2;
    static int yPrevious = config.displayHeight / 2;
    
    if (!_isLocked) {
        if (xPosition < xPrevious) {
            _angleH -= _maxSpeed;
        }
        else if (xPosition > xPrevious) {
            _angleH += _maxSpeed;
        }
        
        if (yPosition < yPrevious) {
            _angleV += _maxSpeed / 1.5f;
        }
        else if (yPosition > yPrevious) {
            _angleV -= _maxSpeed / 1.5f;
        }
        
        if ((xPosition > 1) && (xPosition < (config.displayWidth - 1)))
            xPrevious = xPosition;
        
        if ((yPosition > 1) && (yPosition < (config.displayHeight - 1)))
            yPrevious = yPosition;
    }
}

void DGCameraManager::init() {
    _canBreathe = false;
    _canWalk = false;
    _isLocked = false;
    
    _accelH = 0.0f;
    _accelV = 0.0f;
    
    _angleH = 0.0f;
    _angleV = 0.0f;
    
    _angleHPrevious = 0.0f;
    _angleVPrevious = 0.0f;
    
    _angleHLimit = (float)M_PI * 2;
    _angleVLimit = (float)M_PI / 2;
    
    _bob.currentFactor = DGCamBreatheFactor;
    _bob.currentSpeed = DGCamBreatheSpeed;
    _bob.direction = DGCamPause;
    _bob.displace = 0.0f;
    _bob.expireStrength = 0.0f;
    _bob.inspireStrength = 0.0f;
    _bob.nextPause = 0.0f;
    _bob.position = 0.0f;
    _bob.state = DGCamIdle;
    _bob.timer = 0.0f;
    
    _deltaX = 0;
    _deltaY = 0;
    
    _fovCurrent = DGCamFieldOfView;
    _fovNormal = DGCamFieldOfView;
    _fovPrevious = DGCamFieldOfView;
    
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
    
    _inertia = 1 / (float)DGCamInertia;
    _inOrthoView = false;
    _isPanning = false;
    
    _motionDown = 0.0f;
    _motionLeft = 0.0f;
    _motionRight = 0.0f;
    _motionUp = 0.0f;
    
    _maxSpeed = 1.0f / (float)DGCamMaxSpeed;
    _speedH = 0.0f;
    _speedV = 0.0f;
    _speedFactor = DGCamSpeedFactor;
    
    _isInitialized = true;
}

void DGCameraManager::pan(int xPosition, int yPosition) {
    if (!_isLocked) {
        if (xPosition != kCurrent) {
            if (xPosition > _panRegion.size.width)
                _deltaX = xPosition - _panRegion.size.width;
            else if (xPosition < _panRegion.origin.x)
                _deltaX = xPosition - _panRegion.origin.x;
            else _deltaX = 0.0f;
        }
            
        if (yPosition != kCurrent) {
            if (yPosition > _panRegion.size.height)
                _deltaY = _panRegion.size.height - yPosition;
            else if (yPosition < _panRegion.origin.y)
                _deltaY = _panRegion.origin.y - yPosition;
            else _deltaY = 0.0f;
        }
        
        _speedH = fabs((float)_deltaX) / (float)_speedFactor;
        _speedV = fabs((float)_deltaY) / (float)_speedFactor;
    }
}

void DGCameraManager::panToTargetAngle() {
    if (!_isLocked) {
        if (_angleH < (_angleHTarget - _targetHError) || _angleH > (_angleHTarget + _targetHError)) {
            _deltaX = (_angleHTarget - _angleH) * 360;
            _speedH = fabs((float)_deltaX) / (float)_speedFactor;
        }
        else {
            _deltaX = 0;
            _angleHTarget = _angleH;
        }

        if (_angleV < (_angleVTarget - _targetVError) || _angleV > (_angleVTarget + _targetVError)) {
            _deltaY = (_angleVTarget - _angleV) * 360;
            _speedV = fabs((float)_deltaY) / (float)_speedFactor;
        }
        else {
            _deltaY = 0;
            _angleVTarget = _angleV;
        }
    }
}

void DGCameraManager::simulateWalk() {
    if (_canWalk) {
        _bob.currentFactor = DGCamWalkFactor;
        _bob.currentSpeed = DGCamWalkSpeed;   
        
        _fovCurrent = _fovNormal + (float)DGCamWalkZoomIn;
        
        _bob.state = DGCamWalking;
    }
}

void DGCameraManager::stopPanning() {
    this->pan(config.displayWidth / 2, config.displayHeight / 2);
}

// For the DRAG mode

void DGCameraManager::startDragging(int xPosition, int yPosition) {
    _panRegion.origin.x = xPosition - _dragNeutralZone;
    _panRegion.origin.y = yPosition - _dragNeutralZone;
    _panRegion.size.width = xPosition + _dragNeutralZone;
    _panRegion.size.height = yPosition + _dragNeutralZone;
}

void DGCameraManager::stopDragging() {
    _panRegion.origin.x = (_viewport.width / 2) - _dragNeutralZone;
    _panRegion.origin.y = (_viewport.height / 2) - _dragNeutralZone;
    _panRegion.size.width = (_viewport.width / 2) + _dragNeutralZone;
    _panRegion.size.height = (_viewport.height / 2) + _dragNeutralZone;
    
    this->pan(config.displayWidth / 2, config.displayHeight / 2);
}

void DGCameraManager::lock() {
    if (!_isLocked) {
        this->stopPanning();
        
        _motionDown = 0;
        _motionUp = 0;
        _motionLeft = 0;
        _motionRight = 0;
        
        _angleHPrevious = _angleH;
        _angleVPrevious = _angleV;
        _fovPrevious = _fovCurrent;
        
        _angleH = 0.0f;
        _angleV = 0.0f;
        
        _isLocked = true;
    }
}

bool DGCameraManager::isLocked() {
    return _isLocked;
}

void DGCameraManager::unlock() {
    _angleH = _angleHPrevious;
    _angleV = _angleVPrevious;
    
    _isLocked = false;
}

void DGCameraManager::update() {
    _isPanning = false;
    
    // Calculate horizontal motion
    if (_deltaX > 0) {
        if (_motionRight < _maxSpeed)
            _motionRight += _speedH;
        
        // Apply inertia to opposite direction
        if (_motionLeft > 0.0f)
            _motionLeft -= _inertia;
        
        _isPanning = true;
    }
	else if (_deltaX < 0) {
        if (_motionLeft < _maxSpeed)
            _motionLeft += _speedH;
        
        // Inertia        
        if (_motionRight > 0.0f)
            _motionRight -= _inertia;        
        
        _isPanning = true;
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
        
        _isPanning = true;
    }
	else if (_deltaY < 0) {
        if (_motionUp < _maxSpeed)
            _motionUp += _speedV;
        
        // Inertia
        if (_motionDown > 0.0f)
            _motionDown -= _inertia;
        
        _isPanning = true;
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
        else{
            _accelH = 0.0f;
        }
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
        _angleH -= sin(_accelH) * _motionLeft * config.globalSpeed();
    
    if (_motionRight)
        _angleH += sin(_accelH) * _motionRight * config.globalSpeed();
    
    // Apply vertical motion
    if (_motionDown)
        _angleV += sin(_accelV) * _motionDown * config.globalSpeed();
    
    if (_motionUp)
        _angleV -= sin(_accelV) * _motionUp * config.globalSpeed();
    
    // Limit horizontal rotation
    if (_angleH > (GLfloat)_angleHLimit)
        _angleH = 0.0f;
    else if (_angleH < 0.0f)
        _angleH = (GLfloat)_angleHLimit;
    
    // Limit vertical rotation
    if (_angleV > (GLfloat)_angleVLimit) {
        _angleV = (GLfloat)_angleVLimit;
        _motionUp = 0.0f;
        _motionDown = 0.0f;
    }
    else if (_angleV < -(GLfloat)_angleVLimit) {
        _angleV = -(GLfloat)_angleVLimit;
        _motionUp = 0.0f;
        _motionDown = 0.0f;
    }
    
    _orientation[0] = (float)sin(_angleH);
    _orientation[1] = _angleV;
    _orientation[2] = (float)-cos(_angleH);
    
    if (_bob.state != DGCamIdle)
        _calculateBob();
    
    if (_isInitialized) {    
        gluLookAt(_position[0], _position[1] + (_bob.displace / 4), _position[2],
                  _orientation[0], _orientation[1] + _bob.displace, _orientation[2],
                  _orientation[3], _orientation[4], _orientation[5]);
    }
    
    // Displace in x for scare
    /*gluLookAt(_position[0], _position[1], _position[2],
              _orientation[0] + sin(_bob.displace), _orientation[1], _orientation[2],
              _orientation[3], _orientation[4], _orientation[5]);*/
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void DGCameraManager::_calculateBob() {
    switch (_bob.state) {
        case DGCamBreathing:
            if (_bob.currentFactor > DGCamBreatheFactor)
                _bob.currentFactor--;
            else if (_bob.currentFactor < DGCamBreatheFactor)
                _bob.currentFactor++;
            
            if (_bob.currentSpeed > DGCamBreatheSpeed)
                _bob.currentSpeed--;
            else if (_bob.currentSpeed < DGCamBreatheSpeed)
                _bob.currentSpeed++;
            break;            
        case DGCamScared:
            // Nothing to do here
            break;            
        case DGCamWalking:
            // Perform walk FX operations
            if (_fovCurrent > _fovNormal)
                _fovCurrent -= (10.0 / DGCamWalkZoomIn) * config.globalSpeed();
            else {
                _fovCurrent = _fovNormal;
                if (_canBreathe)
                    _bob.state = DGCamBreathing;
                else _bob.state = DGCamIdle;
            }
            
            this->setViewport(config.displayWidth, config.displayHeight);
            break;
    }
    
    switch (_bob.direction) {
        case DGCamPause:
            _bob.timer += 0.1f;
            if (_bob.timer >= _bob.nextPause) {
                _bob.timer = 0.0f;
                _bob.nextPause = 0.0f;
                _bob.inspireStrength = ((float)((rand() % 3) + 5)) / 10.0f;
                _bob.direction = DGCamInspire;
            }
            break;
        case DGCamInspire:
            if (_bob.position < _bob.inspireStrength) 
                _bob.position += (_bob.inspireStrength / _bob.currentSpeed);
            else {
                _bob.expireStrength = ((float)((rand() % 2) + 6)) / 10.0f;
                _bob.direction = DGCamExpire;
            }
            
            _bob.displace = sin(_bob.position) * cos(_bob.position) / _bob.currentFactor;
            break;
        case DGCamExpire:
            if (_bob.position > -_bob.expireStrength)
                _bob.position -= (_bob.expireStrength / _bob.currentSpeed);
            else {
                _bob.direction = DGCamPause;
                if (_bob.state != DGCamBreathing)
                    _bob.nextPause = 0;
                else
                    _bob.nextPause = (float)((rand() % 3) + 1); // For making the breathing more irregular
            }
            
            _bob.displace = sin(_bob.position) * cos(_bob.position) / _bob.currentFactor;
            break;    
    }
}

int DGCameraManager::_toDegrees(float angle, float limit) {
    int degrees = (angle * 360.0f) / limit;
    return degrees;
}

float DGCameraManager::_toRadians(float angle, float limit) {
    float radians = (angle * limit) / 360.0f;
    return radians;
}
