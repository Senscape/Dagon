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

#include <string.h>
#include "DGConfig.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGConfig::DGConfig() {
    strcpy(_appPath, "");
    strncpy(_resPath, DGDefCatalogPath, DGMaxPathLength);
	strncpy(_scriptName, DGDefScriptFile, DGMaxFileLength);
    strcpy(_userPath, "");
    strncpy(_texExtension, DGDefTexExtension, 4);
	
	antialiasing = DGDefAntialiasing;
    bundleEnabled = DGDefBundleEnabled;
	displayWidth = DGDefDisplayWidth;
	displayHeight = DGDefDisplayHeight;
	displayDepth = DGDefDisplayDepth;
	debugMode = DGDefDebugMode;
	dust = DGDefDust;
	fullScreen = DGDefFullScreen;
	effects = DGDefEffects;
	log = DGDefLog;
	framerate = DGDefFramerate;
	showSplash = DGDefShowSplash;
	showSpots = DGDefShowSpots;
	texCompression = DGDefTexCompression;
    scare = DGDefScare;
    throb = DGDefThrob;
	verticalSync = DGDefVerticalSync;
	
	//TODO: If vertical sync is enabled this should not be necessary!
	// This is a readonly setting
	_globalSpeed = 60.0f / (float)framerate;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGConfig::~DGConfig() {
    // Nothing to do here
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

float DGConfig::globalSpeed() {
    return _globalSpeed;
}

const char*	DGConfig::path(int ofType, const char* forFile) {
 	static char fullPath[DGMaxPathLength + DGMaxFileLength];
	
    switch (ofType) {
        case DGPathApp:
            strncpy(fullPath, _appPath, DGMaxPathLength);
            break;
        case DGPathRes:
            strncpy(fullPath, _resPath, DGMaxPathLength);
            break;
        case DGPathUser:
            strncpy(fullPath, _userPath, DGMaxPathLength);
            break;
    }
	
	strncat(fullPath, forFile, DGMaxFileLength);
	
	return fullPath;   
}

void DGConfig::setPath(int forType, const char* path) {
    switch (forType) {
        case DGPathApp:
            strncpy(_appPath, path, DGMaxPathLength);
            break;
        case DGPathRes:
            strncpy(_resPath, path, DGMaxPathLength);
            break;
        case DGPathUser:
            strncpy(_userPath, path, DGMaxPathLength);
            break;
    }
}

const char*	DGConfig::script() {
    return _scriptName;
}

void DGConfig::setScript(const char* name) {
    strncpy(_scriptName, name, DGMaxFileLength);
}

const char*	DGConfig::texExtension() {
    return _texExtension;
}

void DGConfig::setTexExtension(const char* ext) {
    strncpy(_texExtension, ext, 4);
    
    if (!strncmp(_texExtension, "tex", 4))
        bundleEnabled = true;
    else
        bundleEnabled = false;
}
