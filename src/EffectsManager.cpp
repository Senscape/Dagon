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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "CameraManager.h"
#include "Config.h"
#include "EffectsManager.h"
#include "Texture.h"
#include "TimerManager.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

EffectsManager::EffectsManager() :
cameraManager(CameraManager::instance()),
config(Config::instance()),
timerManager(TimerManager::instance())
{
  const char* Names[] = { "brightness", "contrast", "saturation",
    "dust", "dustColor", "dustSize", "dustSpeed", "dustSpread",
    "motionBlur", "noise", "sepia", "sharpen", "sharpenRatio",
    "throb", "throbStyle" };
  
  const size_t len = sizeof(Names) / sizeof(Names[0]);
  this->initAliases(len, Names);
  
  set("brightness", 100);
  set("contrast", 100);
  set("saturation", 100);
  set("dust", 0);
  _theSettings["dustColor"].value = 0xffeeddaa;
  set("dustSize", 2);
  set("dustSpeed", 1);
  set("dustSpread", 20);
  set("motionBlur", 4);
  set("noise", 350);
  set("sepia", 0);
  set("sharpen", 0);
  set("sharpenRatio", 25);
  set("throb", 0);
  set("throbStyle", 1);
  
  _calculateDustData();
  
  _isActive = false;
  _isInitialized = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

EffectsManager::~EffectsManager() {
  this->pause();
  
  if (_isInitialized) {
    glDetachShader(_program, _fragment);
    glDeleteShader(_fragment);
    glDeleteProgram(_program);
    
    delete _dustTexture;
    
    _isActive = false;
    _isInitialized = false;
  }
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////
  
void EffectsManager::_updateShader(int theEffect, float withValue) {
  if (_isInitialized) {
    GLint parameter;
    GLint intensity;
    
    play();
    
    switch (theEffect) {
      case effects::kBrightness:
        intensity = glGetUniformLocation(_program, "AdjustBrightness");
        withValue = withValue / 100.0f;
        glUniform1f(intensity, withValue);
        break;
        
      case effects::kSaturation:
        intensity = glGetUniformLocation(_program, "AdjustSaturation");
        withValue = withValue / 100.0f;
        glUniform1f(intensity, withValue);
        break;
        
      case effects::kContrast:
        intensity = glGetUniformLocation(_program, "AdjustContrast");
        withValue = withValue / 100.0f;
        glUniform1f(intensity, withValue);
        break;
        
      case effects::kMotionBlur:
        parameter = glGetUniformLocation(_program, "MotionBlurEnabled");
        intensity = glGetUniformLocation(_program, "MotionBlurIntensity");
        if (withValue)
          glUniform1i(parameter, true);
        else
          glUniform1i(parameter, false);
        withValue = (10 - withValue) / 1.0f;
        glUniform1f(intensity, withValue);
        break;
        
      case effects::kNoise:
        parameter = glGetUniformLocation(_program, "NoiseEnabled");
        intensity = glGetUniformLocation(_program, "NoiseIntensity");
        if (withValue)
          glUniform1i(parameter, true);
        else
          glUniform1i(parameter, false);
        withValue = withValue / 100.0f;
        glUniform1f(intensity, withValue);
        break;
        
      case effects::kSepia:
        parameter = glGetUniformLocation(_program, "SepiaEnabled");
        intensity = glGetUniformLocation(_program, "SepiaIntensity");
        if (withValue)
          glUniform1i(parameter, true);
        else
          glUniform1i(parameter, false);
        withValue = withValue / 100.0f;
        glUniform1f(intensity, withValue);
        break;
        
      case effects::kSharpenRatio:
        intensity = glGetUniformLocation(_program, "SharpenRatio");
        withValue = withValue / 100.0f;
        glUniform1f(intensity, withValue);
        break;
        
      case effects::kSharpen:
        parameter = glGetUniformLocation(_program, "SharpenEnabled");
        intensity = glGetUniformLocation(_program, "SharpenIntensity");
        if (withValue)
          glUniform1i(parameter, true);
        else
          glUniform1i(parameter, false);
        withValue = withValue / 10.0f;
        glUniform1f(intensity, withValue);
        break;
        
      case effects::kThrob:
        if (!withValue) {
          // Special case, we reset the brightness and contrast
          intensity = glGetUniformLocation(_program, "AdjustBrightness");
          glUniform1f(intensity, this->get("brightness") / 100.0f);
          intensity = glGetUniformLocation(_program, "AdjustContrast");
          glUniform1f(intensity, this->get("contrast") / 100.0f);
        }
        break;
        
      case effects::kDust:
      case effects::kDustColor:
      case effects::kDustSize:
      case effects::kDustSpeed:
      case effects::kDustSpread:
        _calculateDustData();
        break;
        
      default:
        break;
    }
    
    // Special case to enable/disable adjustment if three values are normal
    if ((this->get("brightness") != 100) ||
        (this->get("contrast") != 100) ||
        (this->get("saturation") != 100)) {
      parameter = glGetUniformLocation(_program, "AdjustEnabled");
      glUniform1i(parameter, true);
    }
    else {
      parameter = glGetUniformLocation(_program, "AdjustEnabled");
      glUniform1i(parameter, false);
    }
    
    pause();
  }
}


void EffectsManager::drawDust() {
  if (this->get("dust") && config.effects) {
    // Temporary
    glPushMatrix();
    
    uint32_t aux = _theSettings["dustColor"].value;
    uint8_t r = (aux & 0xff000000) >> 24;
    uint8_t g = (aux & 0x00ff0000) >> 16;
    uint8_t b = (aux & 0x0000ff00) >> 8;
    uint8_t a = (aux & 0x000000ff);
    
    glColor4f((float)(r / 255.0f), (float)(g / 255.0f), (float)(b / 255.0f), (float)(a / 255.f));
    
    _dustTexture->bind();
    for (int i = 0; i < _dustData.numOfParticles; i++) {
      _particles[i].x += _particles[i].xd / _dustData.speed;
      _particles[i].y += _particles[i].yd / _dustData.speed;
      _particles[i].z += _particles[i].zd / _dustData.speed;
      if (_particles[i].y <= -0.5f) {
        _buildParticle(i);
      }
      
      GLfloat texCoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
      glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
      
      glRotatef(1.0f, 0.0f, 1.0f, 0.0f);
      
      GLfloat quadCoords[] = {
        _particles[i].x, _particles[i].y, _particles[i].z + _dustData.size,
        _particles[i].x, _particles[i].y + _dustData.size, _particles[i].z + _dustData.size,
        _particles[i].x + _dustData.size, _particles[i].y + _dustData.size, _particles[i].z + _dustData.size,
        _particles[i].x + _dustData.size, _particles[i].y, _particles[i].z + _dustData.size };
      
      glVertexPointer(3, GL_FLOAT, 0, quadCoords);
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    
    glPopMatrix();
  }
}

void EffectsManager::init() {
  const char* pointerToData;
  
  if (kEffectsReadFromFile) {
    if (_textFileRead()) {
      pointerToData = _shaderData;
    }
    else return;
  }
  else pointerToData = kShaderData;
  
  _fragment = glCreateShader(GL_FRAGMENT_SHADER);
  
  glShaderSource(_fragment, 1, &pointerToData, NULL);
  glCompileShader(_fragment);
  
  _program = glCreateProgram();
  glAttachShader(_program, _fragment);
  
  glLinkProgram(_program);
  
  _isInitialized = true;
  
  // Initialize dust
  for (int i = 0; i < kEffectsMaxDust; ++i) {
    _buildParticle(i);
  }
  
  _dustTexture = new Texture;
  _dustTexture->loadFromMemory(kDustData, 3666);
}
  
void EffectsManager::loadSettings(const SettingCollection& theSettings) {
  Configurable::loadSettings(theSettings);
  
  if (beginIteratingSettings()) {
    Setting theEffect;
    while (iterateSettings()) {
      std::string theName = getCurrentSetting(&theEffect);
      int key = Configurable::indexOf(theName);
      Configurable::set(theName, theEffect.value);
      _updateShader(key, theEffect.value);
    }
  }
}

void EffectsManager::pause() {
  if (_isActive) {
    glUseProgram(0);
    _isActive = false;
  }
}

void EffectsManager::play() {
  if (_isInitialized && !_isActive) {
    glUseProgram(_program);
    
    _isActive = true;
  }
}

void EffectsManager::set(const std::string& theName, float theValue) {
  int key = Configurable::indexOf(theName);
  Configurable::set(theName, theValue);
  _updateShader(key, theValue);
}

void EffectsManager::update() {
  static float noise = 0.0f;
  
  if (_isActive) {
    GLint parameter;
    
    if (this->get("motionBlur")) {
      parameter = glGetUniformLocation(_program, "MotionBlurOffsetX");
      glUniform1f(parameter, cameraManager.motionHorizontal());
      
      parameter = glGetUniformLocation(_program, "MotionBlurOffsetY");
      glUniform1f(parameter, cameraManager.motionVertical());
    }
    
    if (this->get("noise")) {
      parameter = glGetUniformLocation(_program, "NoiseRand");
      glUniform1f(parameter, noise);
      
      if (noise < 1.0f)
        noise += 0.01f;
      else noise = 0.0f;
    }
    
    if (this->get("throb")) {
      static int handlerStyle1 = timerManager.createManual(0.1f, 100);
      static int handlerStyle2 = timerManager.createManual(2, 1000);
      static float aux;
      static float j = 1.0f;
      float internalIntensity = (100 - this->get("throb"));
      
      switch (this->get("throbStyle")) {
        case 1:
          if (timerManager.checkManual(handlerStyle1, 100)) {
            aux = (rand() % 10) - (rand() % 10);
            parameter = glGetUniformLocation(_program, "AdjustBrightness");
            glUniform1f(parameter, (this->get("brightness") / 100.0f) + (aux / internalIntensity)); // Suggested: 50
            
            aux = rand() % 10;
            parameter = glGetUniformLocation(_program, "AdjustContrast");
            glUniform1f(parameter, (this->get("contrast") / 100.0f) + (aux / internalIntensity));
          }
          break;
          
        case 2:
          parameter = glGetUniformLocation(_program, "AdjustBrightness");
          glUniform1f(parameter, (this->get("brightness") / 100.0f) + (aux * j));
          
          parameter = glGetUniformLocation(_program, "AdjustContrast");
          glUniform1f(parameter, (this->get("contrast") / 100.0f) + 0.15f);
          
          if (j > 0)
            j -= 0.1f;
          
          if (timerManager.checkManual(handlerStyle2, 1000)) {
            aux = (float)((rand() % 50) + 5) / internalIntensity; // Suggested: 10
            j = 1.0f;
          }
          break;
      }
    }
  }
}


////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////
  
void EffectsManager::_calculateDustData() {
  _dustData.numOfParticles = this->get("dust") * 100;
  
  if (_dustData.numOfParticles > kEffectsMaxDust)
    _dustData.numOfParticles = kEffectsMaxDust;
  
  _dustData.size = this->get("dustSize") / 2000.0f;
  _dustData.speed = 100 - this->get("dustSpeed");
  _dustData.spread = 100 - this->get("dustSpread");
}

void EffectsManager::_buildParticle(int idx) {
  int s;
  
  s = rand() % (int)kEffectsDustFactor;
  _particles[idx].xd = -(s / kEffectsDustFactor - 0.5f) / _dustData.spread;
  
  s = rand() % (int)kEffectsDustFactor;
  _particles[idx].zd = -(s / kEffectsDustFactor - 0.5f) / _dustData.spread;
  
  s = rand() % (int)kEffectsDustFactor;
  _particles[idx].yd = -s / kEffectsDustFactor / _dustData.spread;
  
  s = rand() % (int)kEffectsDustFactor;
  _particles[idx].x = s / kEffectsDustFactor - 0.5f;
  
  s = rand() % (int)kEffectsDustFactor;
  _particles[idx].y = s / kEffectsDustFactor;
  
  s = rand() % (int)kEffectsDustFactor;
  _particles[idx].z = s / kEffectsDustFactor - 0.5f;
}

// Modified example from Lighthouse 3D: http://www.lighthouse3d.com
bool EffectsManager::_textFileRead() {
  FILE* fh;
  long size = 0;
  
  fh = fopen(kEffectsFileName, "rt");
  if (fh != NULL) {
    
    fseek(fh, 0, SEEK_END);
    size = ftell(fh);
    rewind(fh);
    
    if (size > 0) {
      _shaderData = (char*)malloc(sizeof(char) * (size + 1));
      size = fread(_shaderData, sizeof(char), size, fh);
      _shaderData[size] = '\0';
    }
    
    fclose(fh);
    
    return true;
  }
  
  return false;
}
  
}
