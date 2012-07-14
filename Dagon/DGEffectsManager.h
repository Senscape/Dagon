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

enum DGEffects {
    DGEffectAdjust,
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
    DGEffectMotionBlurIntensity,
    DGEffectNoiseIntensity,
    DGEffectSepiaIntensity,    
    DGEffectSharpenRatio,
    DGEffectSharpenIntensity,
    DGEffectThrobStyle,
    DGEffectThrobIntensity
};

class DGCameraManager;
class DGConfig;
class DGTimerManager;

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
    
    char* _shaderData;

    bool _adjustEnabled;
    float _adjustBrightness;
    float _adjustSaturation;
    float _adjustContrast;
    
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
    
    void init();
    void pause();
    void play();
    void setEnabled(int effectID, bool enabled);
    void setValue(int valueID, float value);
    void update();
    float value(int valueID);
};

#endif // DG_EFFECTSMANAGER_H
