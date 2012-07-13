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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGCameraManager.h"
#include "DGConfig.h"
#include "DGEffectsManager.h"
#include "DGTimerManager.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGEffectsManager::DGEffectsManager() {
    cameraManager = &DGCameraManager::getInstance();
    config = &DGConfig::getInstance();
    timerManager = &DGTimerManager::getInstance();
    
    _adjustEnabled = false;
    _motionBlurEnabled = false;
    _noiseEnabled = false;
    _sepiaEnabled = false;
    _sharpenEnabled = false;
    _throbEnabled = false;
   
    _adjustBrightness = 1.0f;
    _adjustSaturation = 1.0f;
    _adjustContrast = 1.0f;
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

DGEffectsManager::~DGEffectsManager() {
    this->pause();
    
	if (_isInitialized) {
		glDetachShader(_program, _fragment);
		glDeleteShader(_fragment);
		glDeleteProgram(_program);
        
		_isActive = false;
        _isInitialized = false;
	}    
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGEffectsManager::init() {
    const char* pointerToData;
    
    if (DGEffectsReadFromFile) {
        if (_textFileRead()) {
            pointerToData = _shaderData;
        }
        else return;
    }
    else pointerToData = DGDefShaderData;
    
    _fragment = glCreateShader(GL_FRAGMENT_SHADER);	

    glShaderSource(_fragment, 1, &pointerToData, NULL);
    glCompileShader(_fragment);
    
    _program = glCreateProgram();
    glAttachShader(_program, _fragment);
    
    glLinkProgram(_program);
    
    _isInitialized = true;
    
    // Set default values
    this->setValue(DGEffectAdjustBrightness, _adjustBrightness);
    this->setValue(DGEffectAdjustContrast, _adjustContrast);    
    this->setValue(DGEffectAdjustSaturation, _adjustSaturation);
    this->setValue(DGEffectMotionBlurIntensity, _motionBlurIntensity); // Lowest
    this->setValue(DGEffectNoiseIntensity, _noiseIntensity);
    this->setValue(DGEffectSepiaIntensity, _sepiaIntensity);
    this->setValue(DGEffectSharpenIntensity, _sharpenIntensity);
    this->setValue(DGEffectSharpenRatio, _sharpenRatio);
}

void DGEffectsManager::pause() {
    if (_isActive) {
        glUseProgram(0);
        _isActive = false;
    }
}

void DGEffectsManager::play() {
    if (_isInitialized && !_isActive) {
        glUseProgram(_program);
        
        _isActive = true;
    }
}

void DGEffectsManager::setEnabled(int effectID, bool enabled) {
    if (_isInitialized) {
        GLint parameter;
        
        this->play();
        
        switch (effectID) {
            case DGEffectAdjust:
                _adjustEnabled = enabled;
                parameter = glGetUniformLocation(_program, "AdjustEnabled");
                break;
                
            case DGEffectMotionBlur:
                _motionBlurEnabled = enabled;
                parameter = glGetUniformLocation(_program, "MotionBlurEnabled");
                break;
                
            case DGEffectNoise:
                _noiseEnabled = enabled;
                parameter = glGetUniformLocation(_program, "NoiseEnabled");
                break;
                
            case DGEffectSepia:
                _sepiaEnabled = enabled;
                parameter = glGetUniformLocation(_program, "SepiaEnabled");
                break;
                
            case DGEffectSharpen:
                _sharpenEnabled = enabled;
                parameter = glGetUniformLocation(_program, "SharpenEnabled");
                break;
                
            case DGEffectThrob:
                _throbEnabled = enabled;
                
                if (!_throbEnabled) {
                    // Special case, we reset the brightness and contrast
                    this->setValue(DGEffectAdjustBrightness, _adjustBrightness);
                    this->setValue(DGEffectAdjustContrast, _adjustContrast);
                }
                
                this->pause();
                return;
                
            default:
                this->pause();
                return;
        }
        
        glUniform1i(parameter, enabled);
        
        this->pause();
    }
}

void DGEffectsManager::setValue(int valueID, float value) {
    if (_isInitialized) {
        GLint parameter;
        
        this->play();
        
        switch (valueID) {
            case DGEffectAdjustBrightness:
                parameter = glGetUniformLocation(_program, "AdjustBrightness");
                _adjustBrightness = value;
                break;
                
            case DGEffectAdjustSaturation:
                parameter = glGetUniformLocation(_program, "AdjustSaturation");
                _adjustSaturation = value;
                break;
                
            case DGEffectAdjustContrast:
                parameter = glGetUniformLocation(_program, "AdjustContrast");
                _adjustContrast = value;
                break;
                
            case DGEffectMotionBlurIntensity:
                parameter = glGetUniformLocation(_program, "MotionBlurIntensity");
                _motionBlurIntensity = value;
                break;
                
            case DGEffectNoiseIntensity:
                parameter = glGetUniformLocation(_program, "NoiseIntensity");
                _noiseIntensity = value;
                break;
                
            case DGEffectSepiaIntensity:
                parameter = glGetUniformLocation(_program, "SepiaIntensity");
                _sepiaIntensity = value;
                break;
                
            case DGEffectSharpenRatio:
                parameter = glGetUniformLocation(_program, "SharpenRatio");
                _sharpenRatio = value;
                break;
                
            case DGEffectSharpenIntensity:
                parameter = glGetUniformLocation(_program, "SharpenIntensity");
                _sharpenIntensity = value;
                break;
                
            case DGEffectThrobStyle:
                _throbStyle = (int)value;
                this->pause();
                return;
                
            case DGEffectThrobIntensity:
                _throbIntensity = value;
                this->pause();
                return;
                
            default:
                this->pause();
                return;
        }
        
        glUniform1f(parameter, value);
        
        this->pause();
    }
}

void DGEffectsManager::update() {
    static float noise = 0.0f;
    
    if (_isActive) {
        GLfloat parameter;
        
        if (_motionBlurEnabled) {
            parameter = glGetUniformLocation(_program, "MotionBlurOffsetX");
            glUniform1f(parameter, cameraManager->motionHorizontal());
            
            parameter = glGetUniformLocation(_program, "MotionBlurOffsetY");
            glUniform1f(parameter, cameraManager->motionVertical());
        }
        
        if (_noiseEnabled) {
            parameter = glGetUniformLocation(_program, "NoiseRand");
            glUniform1f(parameter, noise);
            
            if (noise < 1.0f)
                noise += 0.01f;
            else noise = 0.0f;
        }
        
        if (_throbEnabled) {
            static int handlerStyle1 = timerManager->createManual(0.1f);
            static int handlerStyle2 = timerManager->createManual(2);
            static float aux;
            static float j = 1.0f;
            
            switch (_throbStyle) {
                case 1:
                    if (timerManager->checkManual(handlerStyle1)) {
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
                     
                    if (timerManager->checkManual(handlerStyle2)) {
                        aux = (float)((rand() % 50) + 5) / _throbIntensity; // Suggested: 10
                        j = 1.0f;
                    }
                    break;
            }
        }
    }
}

float DGEffectsManager::value(int valueID) {
    switch (valueID) {
        case DGEffectAdjustBrightness: return _adjustBrightness;
        case DGEffectAdjustSaturation: return _adjustSaturation;
        case DGEffectAdjustContrast: return _adjustContrast;
        case DGEffectMotionBlurIntensity: return _motionBlurIntensity;
        case DGEffectNoiseIntensity: return _noiseIntensity;
        case DGEffectSepiaIntensity: return _sepiaIntensity;
        case DGEffectSharpenRatio: return _sharpenRatio;
        case DGEffectSharpenIntensity: return _sharpenIntensity;
        case DGEffectThrobStyle: return _throbStyle;
        case DGEffectThrobIntensity: return _throbIntensity;
    }
    
    return 0.0f;
}


////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

// Modified from Lighthouse 3D
bool DGEffectsManager::_textFileRead() {
	FILE* fh;
	long size = 0;
	
    fh = fopen(DGEffectsFileName, "rt");
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
