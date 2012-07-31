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

#ifndef DG_CAMERAMANAGER_H
#define DG_CAMERAMANAGER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum DGCamBobDirections {
    DGCamExpire,
    DGCamInspire,
    DGCamPause
};

enum DGCamBobStates {
    DGCamIdle,
    DGCamBreathing,
    DGCamScared,
    DGCamWalking
};

enum DGCamDefaults {
    DGCamAccelerationFactor = 250,
    DGCamDragNeutralZone = 100,    
	DGCamFieldOfView = 55,
    DGCamFreeNeutralZone = 300,
    DGCamInertia = 350, // Maintain motion after panning stops
    DGCamMaxSpeed = 25,
    DGCamSpeedFactor = 750000 // Make movement smoother, duplicated in Drag mode

};

enum DGCamEffects {
    DGCamBreatheFactor = 35,
    DGCamBreatheSpeed = 100,
    DGCamScareFactor = 15,
    DGCamScareSpeed = 15,     
    DGCamWalkFactor = 20,
    DGCamWalkSpeed = 15,    
    DGCamWalkZoomIn = 30
};

typedef struct {
    int currentFactor;
    int currentSpeed;
    int direction;
    float displace;
    float expireStrength;
    float inspireStrength;
    float nextPause;
    float position;
    int state;
    float timer;
} DGCameraBob;

class DGConfig;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGCameraManager {
    DGConfig* config;

    bool _isInitialized;
    
    bool _canBreathe;
    bool _canWalk;
    
    float _accelH;
    float _accelV;
    
    GLfloat _angleH;
    GLfloat _angleV;
    
    GLfloat _angleHTarget;
    GLfloat _angleVTarget;
    float _targetHError;
    float _targetVError;
    
    GLfloat _angleHLimit;
    GLfloat _angleVLimit;
    
    DGCameraBob _bob;
    
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
    bool _inOrthoView;
    bool _isPanning;
    
    float _motionDown;
    float _motionLeft;
    float _motionRight;
    float _motionUp;    
    
    float _maxSpeed;
    float _speedH;
    float _speedV;
    int _speedFactor;
    
    void _calculateBob();
    int _toDegrees(float angle, float limit);
    float _toRadians(float angle, float limit);
    
    // Private constructor/destructor
    DGCameraManager();
    ~DGCameraManager();
    // Stop the compiler generating methods of copy the object
    DGCameraManager(DGCameraManager const& copy);            // Not implemented
    DGCameraManager& operator=(DGCameraManager const& copy); // Not implemented
    
public:
    static DGCameraManager& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGCameraManager instance;
        return instance;
    }
    
    // Checks
    
    bool canBreathe();
    bool canWalk();
    bool isPanning();
    
    // Gets
    
    int angleHorizontal();
    int angleVertical();
    int cursorWhenPanning();
    float fieldOfView();
    int inertia();
    int maxSpeed();
    float motionHorizontal();
    float motionVertical();
    int neutralZone();
    float* orientation(); // Returns the current angle in vector form
    float* position(); // Returns the position of the camera
    int speedFactor();
    int verticalLimit();
    
    // Sets
    
    void setAngleHorizontal(float horizontal);
    void setAngleVertical(float vertical);
    void setBreathe(bool enabled);
    void setFieldOfView(float fov);
    void setInertia(int inertia);
    void setMaxSpeed(int speed);
    void setNeutralZone(int zone);
    void setSpeedFactor(int speed);
    void setTargetAngle(float horizontal, float vertical);    
    void setVerticalLimit(float limit);
    void setViewport(int width, int height);    
    void setWalk(bool enabled);
    
    // State changes
    
    void beginOrthoView();
    void init();
    void endOrthoView();
    void pan(int xPosition, int yPosition);
    void panToTargetAngle();
    void simulateWalk();
    void stopPanning();

    // For the DRAG mode
    void startDragging(int xPosition, int yPosition); // Recreates the neutral zone
    void stopDragging(); // Resets the neutral zone
    
    void update();
};

#endif // DG_CAMERAMANAGER_H
