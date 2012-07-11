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
#define DGEffectsReadFromFile   1

enum DGEffects {
    DGEffectBlur,
    DGEffectColorize,
    DGEffectContrast,
    DGEffectConvolution,
    DGEffectNoise,
    DGEffectSharpen
};

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGEffectsManager {
    GLuint _fragment;
    GLuint _program;
    
    char* _shaderData;
    
    bool _blurEnabled;
    bool _colorizeEnabled;
    bool _contrastEnabled;
    bool _convolutionEnabled;
    bool _noiseEnabled;
    bool _sharpenEnabled;
    
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
    
    void disable(int ID);    
    void enable(int ID);
    void init();
    void pause();
    void play();    
    void setBlur(int horizontalOffset, int verticalOffset);
    void setGamma(float intensity);    
};

#endif // DG_EFFECTSMANAGER_H
