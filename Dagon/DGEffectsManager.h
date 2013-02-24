////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2012 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
//
////////////////////////////////////////////////////////////

#ifndef DG_EFFECTSMANAGER_H
#define DG_EFFECTSMANAGER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include "DGPlatform.h"

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

class DGCameraManager;
class DGConfig;
class DGTexture;
class DGTimerManager;

// Reference to embedded dust data
extern "C" const unsigned char DGDefDustBinary[];

// Reference to embedded shader data
extern "C" const char DGDefShaderData[];

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGEffectsManager {
    DGConfig* config;
    DGCameraManager* cameraManager;
    DGTimerManager* timerManager;
    
    GLuint _fragment;
    GLuint _program;
    
    DGParticle _particles[DGEffectsMaxDust];
    
    DGTexture* _dustTexture;
    char* _shaderData;

    bool _adjustEnabled;
    float _adjustBrightness;
    float _adjustSaturation;
    float _adjustContrast;
    
    bool _dustEnabled;
    int _dustColor;
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
    
    // Private constructor/destructor
    DGEffectsManager();
    ~DGEffectsManager();
    // Stop the compiler generating methods of copy the object
    DGEffectsManager(DGEffectsManager const& copy);            // Not implemented
    DGEffectsManager& operator=(DGEffectsManager const& copy); // Not implemented
    
public:
    static DGEffectsManager& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGEffectsManager instance;
        return instance;
    }
    
    void drawDust();
    void init();
    bool isEnabled(int effectID);
    void pause();
    void play();
    void setEnabled(int effectID, bool enabled);
    void setValuef(int valueID, float value);
    void setValuei(int valueID, int value);
    void update();
    float value(int valueID);
};

#endif // DG_EFFECTSMANAGER_H
