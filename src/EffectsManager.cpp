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
  _adjustBrightness = 1.0f;
  _adjustSaturation = 1.0f;
  _adjustContrast = 1.0f;
  _dustColor = kColorWhite;
  _dustIntensity = 0.0f;
  _dustSize = 0.0007f;
  _dustSpeed = 99.0f;
  _dustSpread = 200.0f;
  _motionBlurIntensity = 4.0f; // Lowest
  _noiseIntensity = 0.0f;
  _sepiaIntensity = 0.0f;
  _sharpenRatio = 0.25f; // Not currently used in the script, so we set a standard value here
  _sharpenIntensity = 0.0f;
  _throbStyle = 0;
  _throbIntensity = 100.0f; // Lowest
  
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

void EffectsManager::drawDust() {
  if (_dustIntensity && config.effects) {
    // Temporary
    glPushMatrix();
    
    uint32_t aux = _dustColor;
    
    uint8_t b = (aux & 0x000000ff);
    uint8_t g = (aux & 0x0000ff00) >> 8;
    uint8_t r = (aux & 0x00ff0000) >> 16;
    uint8_t a = (aux & 0xff000000) >> 24;
    
    glColor4f((float)(r / 255.0f), (float)(g / 255.0f), (float)(b / 255.0f), (float)(a / 255.f));
    
    _dustTexture->bind();
    for (int i = 0; i < _dustIntensity; i++) {
      _particles[i].x += _particles[i].xd / _dustSpeed;
      _particles[i].y += _particles[i].yd / _dustSpeed;
      _particles[i].z += _particles[i].zd / _dustSpeed;
      if (_particles[i].y <= -0.5f) {
        _buildParticle(i);
      }
      
      GLfloat texCoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
      glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
      
      glRotatef(1.0f, 0.0f, 1.0f, 0.0f);
      
      GLfloat quadCoords[] = {
        _particles[i].x, _particles[i].y, _particles[i].z + _dustSize,
        _particles[i].x, _particles[i].y + _dustSize, _particles[i].z + _dustSize,
        _particles[i].x + _dustSize, _particles[i].y + _dustSize, _particles[i].z + _dustSize,
        _particles[i].x + _dustSize, _particles[i].y, _particles[i].z + _dustSize };
      
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
  
  // Set default values
  this->setValuef(kEffectBrightness, _adjustBrightness);
  this->setValuef(kEffectContrast, _adjustContrast);
  this->setValuef(kEffectSaturation, _adjustSaturation);
  this->setValuef(kEffectMotionBlur, _motionBlurIntensity); // Lowest
  this->setValuef(kEffectNoise, _noiseIntensity);
  this->setValuef(kEffectSepia, _sepiaIntensity);
  this->setValuef(kEffectSharpen, _sharpenIntensity);
  this->setValuef(kEffectSharpenRatio, _sharpenRatio);
  
  // Initialize dust
  for (int i = 0; i < kEffectsMaxDust; ++i) {
    _buildParticle(i);
  }
  
  _dustTexture = new Texture;
  _dustTexture->loadFromMemory(kDustData, 3666);
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

void EffectsManager::setValuef(int valueID, float theValue) {
  if (_isInitialized) {
    GLint parameter;
    GLint intensity;
    
    this->play();
    
    switch (valueID) {
      case kEffectBrightness:
        intensity = glGetUniformLocation(_program, "AdjustBrightness");
        _adjustBrightness = theValue;
        break;
        
      case kEffectSaturation:
        intensity = glGetUniformLocation(_program, "AdjustSaturation");
        _adjustSaturation = theValue;
        break;
        
      case kEffectContrast:
        intensity = glGetUniformLocation(_program, "AdjustContrast");
        _adjustContrast = theValue;
        break;
        
      case kEffectDustColor:
        _dustColor = static_cast<uint32_t>(theValue);
        this->pause();
        return;
        
      case kEffectDust:
        if (theValue < kEffectsMaxDust)
          _dustIntensity = theValue;
        this->pause();
        return;
        
      case kEffectDustSize:
        _dustSize = theValue;
        this->pause();
        return;
        
      case kEffectDustSpeed:
        _dustSpeed = theValue;
        this->pause();
        return;
        
      case kEffectDustSpread:
        _dustSpread = theValue;
        this->pause();
        return;
        
      case kEffectMotionBlur:
        parameter = glGetUniformLocation(_program, "MotionBlurEnabled");
        intensity = glGetUniformLocation(_program, "MotionBlurIntensity");
        _motionBlurIntensity = theValue;
        if (theValue)
          glUniform1i(parameter, true);
        else
          glUniform1i(parameter, false);
        break;
        
      case kEffectNoise:
        parameter = glGetUniformLocation(_program, "NoiseEnabled");
        intensity = glGetUniformLocation(_program, "NoiseIntensity");
        _noiseIntensity = theValue;
        if (theValue)
          glUniform1i(parameter, true);
        else
          glUniform1i(parameter, false);
        break;
        
      case kEffectSepia:
        parameter = glGetUniformLocation(_program, "SepiaEnabled");
        intensity = glGetUniformLocation(_program, "SepiaIntensity");
        _sepiaIntensity = theValue;
        if (theValue)
          glUniform1i(parameter, true);
        else
          glUniform1i(parameter, false);
        break;
        
      case kEffectSharpenRatio:
        intensity = glGetUniformLocation(_program, "SharpenRatio");
        _sharpenRatio = theValue;
        break;
        
      case kEffectSharpen:
        parameter = glGetUniformLocation(_program, "SharpenEnabled");
        intensity = glGetUniformLocation(_program, "SharpenIntensity");
        _sharpenIntensity = theValue;
        if (theValue)
          glUniform1i(parameter, true);
        else
          glUniform1i(parameter, false);
        break;
        
      case kEffectThrobStyle:
        _throbStyle = (int)theValue;
        this->pause();
        return;
        
      case kEffectThrob:
        _throbIntensity = theValue;
        if (!_throbIntensity) {
          // Special case, we reset the brightness and contrast
          intensity = glGetUniformLocation(_program, "AdjustBrightness");
          glUniform1f(intensity, _adjustBrightness);
          intensity = glGetUniformLocation(_program, "AdjustContrast");
          glUniform1f(intensity, _adjustContrast);
        }
        this->pause();
        return;
        
      default:
        this->pause();
        return;
    }
    
    glUniform1f(intensity, theValue);
    
    // Special case to enable/disable adjustment if three values are normal
    if ((fabs(_adjustBrightness - 1.0f) < kEpsilon) &&
        (fabs(_adjustContrast - 1.0f) < kEpsilon) &&
        (fabs(_adjustSaturation - 1.0f) < kEpsilon)) {
      parameter = glGetUniformLocation(_program, "AdjustEnabled");
      glUniform1i(parameter, false);
    }
    else {
      parameter = glGetUniformLocation(_program, "AdjustEnabled");
      glUniform1i(parameter, true);
    }
    
    this->pause();
  }
}

void EffectsManager::setValuei(int valueID, int theValue) {
  if (_isInitialized) {
    switch (valueID) {
      case kEffectDustColor:
        _dustColor = theValue;
        break;
        
      default:
        break;
    }
  }
}

void EffectsManager::update() {
  static float noise = 0.0f;
  
  if (_isActive) {
    GLint parameter;
    
    if (_motionBlurIntensity) {
      parameter = glGetUniformLocation(_program, "MotionBlurOffsetX");
      glUniform1f(parameter, cameraManager.motionHorizontal());
      
      parameter = glGetUniformLocation(_program, "MotionBlurOffsetY");
      glUniform1f(parameter, cameraManager.motionVertical());
    }
    
    if (_noiseIntensity) {
      parameter = glGetUniformLocation(_program, "NoiseRand");
      glUniform1f(parameter, noise);
      
      if (noise < 1.0f)
        noise += 0.01f;
      else noise = 0.0f;
    }
    
    if (_throbIntensity) {
      static int handlerStyle1 = timerManager.createManual(0.1f, 100);
      static int handlerStyle2 = timerManager.createManual(2, 1000);
      static float aux;
      static float j = 1.0f;
      
      switch (_throbStyle) {
        case 1:
          if (timerManager.checkManual(handlerStyle1, 100)) {
            aux = (rand() % 10) - (rand() % 10);
            parameter = glGetUniformLocation(_program, "AdjustBrightness");
            glUniform1f(parameter, _adjustBrightness + (aux / _throbIntensity)); // Suggested: 50
            
            aux = rand() % 10;
            parameter = glGetUniformLocation(_program, "AdjustContrast");
            glUniform1f(parameter, _adjustContrast + (aux / _throbIntensity));
          }
          break;
          
        case 2:
          parameter = glGetUniformLocation(_program, "AdjustBrightness");
          glUniform1f(parameter, _adjustBrightness + (aux * j));
          
          parameter = glGetUniformLocation(_program, "AdjustContrast");
          glUniform1f(parameter, _adjustContrast + 0.15f);
          
          if (j > 0)
            j -= 0.1f;
          
          if (timerManager.checkManual(handlerStyle2, 1000)) {
            aux = (float)((rand() % 50) + 5) / _throbIntensity; // Suggested: 10
            j = 1.0f;
          }
          break;
      }
    }
  }
}

float EffectsManager::value(int valueID) {
  switch (valueID) {
    case kEffectBrightness: return _adjustBrightness;
    case kEffectSaturation: return _adjustSaturation;
    case kEffectContrast: return _adjustContrast;
    case kEffectDustColor: return _dustColor;
    case kEffectDust: return _dustIntensity;
    case kEffectDustSize: return _dustSize;
    case kEffectDustSpeed: return _dustSpeed;
    case kEffectDustSpread: return _dustSpread;
    case kEffectMotionBlur: return _motionBlurIntensity;
    case kEffectNoise: return _noiseIntensity;
    case kEffectSepia: return _sepiaIntensity;
    case kEffectSharpenRatio: return _sharpenRatio;
    case kEffectSharpen: return _sharpenIntensity;
    case kEffectThrobStyle: return _throbStyle;
    case kEffectThrob: return _throbIntensity;
  }
  
  return 0.0f;
}


////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void EffectsManager::_buildParticle(int idx) {
  int s;
  
  s = rand() % (int)kEffectsDustFactor;
  _particles[idx].xd = -(s / kEffectsDustFactor - 0.5f) / _dustSpread;
  
  s = rand() % (int)kEffectsDustFactor;
  _particles[idx].zd = -(s / kEffectsDustFactor - 0.5f) / _dustSpread;
  
  s = rand() % (int)kEffectsDustFactor;
  _particles[idx].yd = -s / kEffectsDustFactor / _dustSpeed;
  
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
