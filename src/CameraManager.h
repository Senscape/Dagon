////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DAGON_CAMERAMANAGER_H_
#define DAGON_CAMERAMANAGER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Platform.h"

namespace dagon {

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
  DGCamAccelerationFactor = 150,
  DGCamDragNeutralZone = 20,
  DGCamFieldOfView = 55,
  DGCamFreeNeutralZone = 40, // Now percentage of screen size
  DGCamInertia = 350, // Maintain motion after panning stops
  DGCamMaxSpeed = 25,
  DGCamSpeedFactor = 750000
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

class Config;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

// TODO: This got fairly messy... Need to separate most of this into another class,
// DGCameraState. Or even merge the camera state with the general State Manager.
// I don't know... every time I rewrote this file, it always got messy. Bloody cameras.

class CameraManager {
  Config& config;
  
  bool _isInitialized;
  bool _isLocked;
  
  bool _canBreathe;
  bool _canWalk;
  
  bool _fovAdjustment;
  
  float _accelH;
  float _accelV;
  
  GLdouble _angleH;
  GLdouble _angleV;
  
  GLdouble _angleHPrevious;
  GLdouble _angleVPrevious;
  
  GLdouble _angleHTarget;
  GLdouble _angleVTarget;
  GLdouble _targetHError;
  GLdouble _targetVError;
  
  GLdouble _angleHLimit;
  GLdouble _angleVLimit;
  
  DGCameraBob _bob;
  
  GLfloat _fovCurrent;
  GLfloat _fovNormal;
  GLfloat _fovPrevious;
  
  int _deltaX;
  int _deltaY;
  
  float _orientation[6];
  float _position[3];
  
  int _dragNeutralZone;
  int _freeNeutralZone;
  
  Size _viewport;
  Rect _panRegion;
  
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
  GLint _toDegrees(GLdouble angle, GLdouble limit);
  GLdouble _toRadians(GLdouble angle, GLdouble limit);
  
  CameraManager();
  CameraManager(CameraManager const&);
  CameraManager& operator=(CameraManager const&);
  ~CameraManager();
  
public:
  static CameraManager& instance() {
    static CameraManager cameraManager;
    return cameraManager;
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
  int horizontalLimit();
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
  void setHorizontalLimit(float limit);  
  void setInertia(int theInertia);
  void setMaxSpeed(int theSpeed);
  void setNeutralZone(int zone);
  void setSpeedFactor(int speed);
  void setTargetAngle(float horizontal, float vertical, bool fovAdjustment);
  void setVerticalLimit(float limit);
  void setViewport(int width, int height);
  void setWalk(bool enabled);
  
  // State changes
  
  void beginOrthoView();
  void directPan(int xPosition, int yPosition);
  void init();
  void endOrthoView();
  void pan(int xPosition, int yPosition);
  void panToTargetAngle();
  void simulateWalk();
  void stopPanning();
  
  // For the DRAG mode
  void startDragging(int xPosition, int yPosition); // Recreates the neutral zone
  void stopDragging(); // Resets the neutral zone
  
  void lock();
  bool isLocked();
  void unlock();
  
  void update();
};
  
}

#endif // DAGON_CAMERAMANAGER_H_
