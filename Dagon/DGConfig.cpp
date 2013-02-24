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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGConfig.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGConfig::DGConfig() {
    strcpy(_appPath, "");
    strncpy(_resPath, DGDefResourcePath, DGMaxPathLength);
	strncpy(_scriptName, DGDefScriptFile, DGMaxFileLength);
    strcpy(_userPath, "");
    strncpy(_texExtension, DGDefTexExtension, 4);
	
	antialiasing = DGDefAntialiasing;
    audioBuffer = DGDefAudioBuffer;
    audioDevice = DGDefAudioDevice;
    autopaths = DGDefAutopaths;
    autorun = DGDefAutorun;
    bundleEnabled = DGDefBundleEnabled;
    controlMode = DGDefControlMode;
	displayWidth = DGDefDisplayWidth;
	displayHeight = DGDefDisplayHeight;
	displayDepth = DGDefDisplayDepth;
	debugMode = DGDefDebugMode;
	forcedFullScreen = DGDefForcedFullScreen;
    framebuffer = DGDefFramebuffer;
    framerate = DGDefFramerate;
	fullScreen = DGDefFullScreen;
	effects = DGDefEffects;
	log = DGDefLog;
    mute = DGDefMute;
    showHelpers = DGDefShowHelpers;
	showSplash = DGDefShowSplash;
	showSpots = DGDefShowSpots;
    silentFeeds = DGDefSilentFeeds;
	texCompression = DGDefTexCompression;
    subtitles = DGDefSubtitles;
	verticalSync = DGDefVerticalSync;
	
    _fps = 0;
    
	// FIXME: This "global speed" setting is sometimes broken on Windows, must implement a frame limiter
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

int DGConfig::framesPerSecond() {
    return _fps;
}

void DGConfig::setFramesPerSecond(int fps) {
    _fps = fps;
}

const char*	DGConfig::path(int ofType, const char* forFile, int andObject) {
 	static char fullPath[DGMaxPathLength + DGMaxFileLength];
	
    switch (ofType) {
        case DGPathApp:
            strncpy(fullPath, _appPath, DGMaxPathLength);
            if (strchr(forFile, '/') == NULL && autopaths) {
                switch (andObject) {
                    case DGObjectRoom:
                        strncat(fullPath, DGDefRoomPath, DGMaxSubPathLength);
                        break;                    
                }
            }
            break;
        case DGPathRes:
            strncpy(fullPath, _resPath, DGMaxPathLength);
            if (strchr(forFile, '/') == NULL && autopaths) {
                switch (andObject) {
                    case DGObjectAudio:
                        strncat(fullPath, DGDefAudioPath, DGMaxSubPathLength);
                        break;
                    case DGObjectCursor:
                        strncat(fullPath, DGDefCursorPath, DGMaxSubPathLength);
                        break;
                    case DGObjectFont:
                        strncat(fullPath, DGDefFontPath, DGMaxSubPathLength);
                        break;                        
                    case DGObjectImage:
                        strncat(fullPath, DGDefImagePath, DGMaxSubPathLength);
                        break;                        
                    case DGObjectNode:
                        strncat(fullPath, DGDefNodePath, DGMaxSubPathLength);
                        break;      
                    case DGObjectVideo:
                        strncat(fullPath, DGDefVideoPath, DGMaxSubPathLength);
                        break;                        
                }
            }
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
