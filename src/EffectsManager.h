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

#include "Configurable.h"
#include "Platform.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define kEffectsFileName       "DGShaderData.fs"
#define kEffectsReadFromFile   0
#define kEffectsMaxDust        10000
#define kEffectsDustFactor     32767.0f

namespace effects {
  
const char* Names[] = { "brightness", "contrast", "saturation",
  "dust", "dustColor", "dustSize", "dustSpeed", "dustSpread",
  "motionBlur", "noise", "sepia", "sharpen", "sharpenRatio",
  "throb", "throbStyle" };

typedef enum {
  kBrightness,
  kContrast,
  kSaturation,
  kDust,
  kDustColor,
  kDustSize,
  kDustSpeed,
  kDustSpread,
  kMotionBlur,
  kNoise,
  kSepia,
  kSharpen,
  kSharpenRatio,
  kThrob,
  kThrobStyle
} Settings;

}

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

class EffectsManager : public Configurable {
  Config& config;
  CameraManager& cameraManager;
  TimerManager& timerManager;
  
  GLuint _fragment;
  GLuint _program;
  DGParticle _particles[kEffectsMaxDust];
  Texture* _dustTexture;
  char* _shaderData;
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
  void pause();
  void play();
  void set(const std::string& theName, int theValue);
  void update();
};
  
}

#endif // DAGON_EFFECTSMANAGER_H_
