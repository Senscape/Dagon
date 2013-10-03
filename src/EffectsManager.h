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

#ifndef DAGON_EFFECTSMANAGER_H_
#define DAGON_EFFECTSMANAGER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <stdint.h>

#include "Platform.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define DGEffectsFileName       "DGShaderData.fs"
#define DGEffectsReadFromFile   0
#define DGEffectsMaxDust        10000
#define DGEffectsDustFactor     32767.0f

enum DGEffects {
  DGEffectAdjust,
  DGEffectDust,
  DGEffectMotionBlur,
  DGEffectNoise,
  DGEffectSepia,
  DGEffectSharpen,
  DGEffectThrob
};

enum DGEffectsValues {
  DGEffectAdjustBrightness,
  DGEffectAdjustSaturation,
  DGEffectAdjustContrast,
  DGEffectDustColor,
  DGEffectDustIntensity,
  DGEffectDustSize,
  DGEffectDustSpeed,
  DGEffectDustSpread,
  DGEffectMotionBlurIntensity,
  DGEffectNoiseIntensity,
  DGEffectSepiaIntensity,
  DGEffectSharpenRatio,
  DGEffectSharpenIntensity,
  DGEffectThrobStyle,
  DGEffectThrobIntensity
};

typedef struct {
  GLfloat x,y,z;
  GLfloat r,g,b;
  GLfloat xd,yd,zd;
  GLfloat cs;
} DGParticle;

class CameraManager;
class Config;
class Texture;
class TimerManager;

// Reference to embedded dust data
extern "C" const unsigned char kDustData[];

// Reference to embedded shader data
extern "C" const char kShaderData[];

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class EffectsManager {
  Config& config;
  CameraManager& cameraManager;
  TimerManager& timerManager;
  
  GLuint _fragment;
  GLuint _program;
  
  DGParticle _particles[DGEffectsMaxDust];
  
  Texture* _dustTexture;
  char* _shaderData;
  
  bool _adjustEnabled;
  float _adjustBrightness;
  float _adjustSaturation;
  float _adjustContrast;
  
  bool _dustEnabled;
  uint32_t _dustColor;
  float _dustIntensity;
  float _dustSize;
  float _dustSpeed;
  float _dustSpread;
  
  bool _motionBlurEnabled;
  float _motionBlurIntensity;
  
  bool _noiseEnabled;
  float _noiseIntensity;
  
  bool _sepiaEnabled;
  float _sepiaIntensity;
  
  bool _sharpenEnabled;
  float _sharpenRatio;
  float _sharpenIntensity;
  
  bool _throbEnabled;
  int _throbStyle;
  float _throbIntensity;
  
  bool _isActive;
  bool _isInitialized;
  
  bool _textFileRead();
  
  void _buildParticle(int idx); // For dust
  
  EffectsManager();
  EffectsManager(EffectsManager const&);
  EffectsManager& operator=(EffectsManager const&);
  ~EffectsManager();
  
public:
  static EffectsManager& instance() {
    static EffectsManager effectsManager;
    return effectsManager;
  }
  
  void drawDust();
  void init();
  bool isEnabled(int effectID);
  void pause();
  void play();
  void setEnabled(int effectID, bool enabled);
  void setValuef(int valueID, float theValue);
  void setValuei(int valueID, int theValue);
  void update();
  float value(int valueID);
};
  
}

#endif // DAGON_EFFECTSMANAGER_H_
