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

char DGDefShaderData[] =
"uniform sampler2D tex;\n"
"uniform float gamma;\n"
"uniform float offsetx;\n"
"uniform float offsety;\n"
"varying vec2 uv;\n"
"\n"
"\n     vec4 Desaturate(vec3 color, float Desaturation) {"
"\n         vec3 grayXfer = vec3(0.3, 0.59, 0.11);"
"\n         vec3 gray = vec3(dot(grayXfer, color));"
"\n         return vec4(mix(color, gray, Desaturation), 1.0);"
"\n     }"
"\n"
"\n     vec3 ContrastSaturationBrightness(vec3 color, float brt, float sat, float con) {"
"\n         const float AvgLumR = 0.5;"
"\n         const float AvgLumG = 0.5;"
"\n         const float AvgLumB = 0.5;"
"\n"
"\n         const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);"
"\n"
"\n         vec3 AvgLumin = vec3(AvgLumR, AvgLumG, AvgLumB);"
"\n         vec3 brtColor = color * brt;"
"\n         vec3 intensity = vec3(dot(brtColor, LumCoeff));"
"\n         vec3 satColor = mix(intensity, brtColor, sat);"
"\n         vec3 conColor = mix(AvgLumin, satColor, con);"
"\n         return conColor;"
"\n     }"
"void main()\n"
"{\n"
"   vec4 pixel = texture2D(tex, gl_TexCoord[0].st);\n"
"	\n"
"	// Sharpen\n"
"	pixel -= texture2D(tex,gl_TexCoord[0].st + 0.00025);\n"
"	pixel += texture2D(tex,gl_TexCoord[0].st - 0.00025);\n"
"    \n"
"	// Motion Blur\n"
"	vec2 offx = vec2(offsetx, 0.0);\n"
"	vec2 offy = vec2(0.0, offsety);\n"
"	\n"
"	vec4 c0 = texture2D(tex,gl_TexCoord[0].st);\n"
"	vec4 c1 = texture2D(tex,gl_TexCoord[0].st - offy);\n"
"	vec4 c2 = texture2D(tex,gl_TexCoord[0].st + offy);\n"
"	vec4 c3 = texture2D(tex,gl_TexCoord[0].st - offx);\n"
"	vec4 c4 = texture2D(tex,gl_TexCoord[0].st + offx);\n"
"	vec4 motion = c0 * 0.2 + c1 * 0.2 + c2 * 0.2 + c3 * 0.2 + c4 * 0.2;\n"
"	\n"
"	// Noise\n"
"	float fract = fract(sin(dot(gl_TexCoord[0].xy, vec2(12.9898,78.233))) * 43758.5453);\n"
"	vec4 noise = vec4(fract * vec4(1.0, 0.8, 0.8, 1.0)) * 0.085 + motion * 0.925; // 0.085 is the intensity\n"
"	\n"
"	// Contrast\n"
"	float gray = pixel.r * 0.39 + pixel.g * 0.50 + pixel.b * 0.11;\n"
"   vec4 contrast = ((gray * 0.1 + pixel * 0.9) * gamma) - vec4(0.15, 0.15, 0.15, 1.0);\n" // Has further brightness 2.5
"  	\n"
//"  	gl_FragColor = mix(contrast, pixel, 0.5);\n"
"   vec3 color = texture2D(tex, gl_TexCoord[0].st).xyz;\n"
"   gl_FragColor = vec4(ContrastSaturationBrightness(color, 2.0, 1.0, 1.1), 1.0);\n"
"}\n";

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGEffectsManager::DGEffectsManager() {
    cameraManager = &DGCameraManager::getInstance();
    config = &DGConfig::getInstance();
    
    _blurEnabled = false;
    _colorizeEnabled = false;
    _contrastEnabled = false;
    _convolutionEnabled = false;
    _noiseEnabled = false;
    _sharpenEnabled = false;
    
    _isActive = false;
    _isInitialized = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGEffectsManager::~DGEffectsManager() {
	if (_isActive) {
		glUseProgram(0);
        
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

void DGEffectsManager::disable(int ID) {
    switch (ID) {
        case DGEffectBlur: _blurEnabled = false; break;
        case DGEffectColorize: _colorizeEnabled = false; break;
        case DGEffectContrast: _contrastEnabled = false; break;
        case DGEffectConvolution: _convolutionEnabled = false; break;
        case DGEffectNoise: _noiseEnabled = false; break;
        case DGEffectSharpen: _sharpenEnabled = false; break;
    }
}

void DGEffectsManager::enable(int ID) {
    switch (ID) {
        case DGEffectBlur: _blurEnabled = true; break;
        case DGEffectColorize: _colorizeEnabled = true; break;
        case DGEffectContrast: _contrastEnabled = true; break;
        case DGEffectConvolution: _convolutionEnabled = true; break;
        case DGEffectNoise: _noiseEnabled = true; break;
        case DGEffectSharpen: _sharpenEnabled = true; break;
    }
}

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
}

void DGEffectsManager::pause() {
   // static float noise = 0.0f;
    if (_isInitialized && _isActive) {
        /*GLfloat rand = glGetUniformLocation(_program, "Rand");
		glUniform1f(rand, noise);
        noise += 0.1f;*/
        
        GLfloat offsetx = glGetUniformLocation(_program, "offsetx");
        glUniform1f(offsetx, cameraManager->motionHorizontal());

        GLfloat offsety = glGetUniformLocation(_program, "offsety");
        glUniform1f(offsety, cameraManager->motionVertical());
        
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

void DGEffectsManager::setBlur(int horizontalOffset, int verticalOffset) {
    if (_isInitialized) {
		GLint ox;
		GLint oy;
        
        GLfloat ox_aux;
        GLfloat oy_aux;
        
        ox_aux = (GLfloat)abs(horizontalOffset); 
        oy_aux = (GLfloat)abs(verticalOffset);
		
		ox = glGetUniformLocation(_program, "offsetx");
		glUniform1f(ox, ox_aux);
		oy = glGetUniformLocation(_program, "offsety");
		glUniform1f(oy, oy_aux);
	}
}

void DGEffectsManager::setGamma(float intensity) {
    if (_isInitialized) {
		GLint ol;
		
		ol = glGetUniformLocation(_program, "gamma");
		glUniform1f(ol, intensity);
	}
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
