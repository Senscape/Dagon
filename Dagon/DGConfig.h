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

#ifndef DG_CONFIG_H
#define DG_CONFIG_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum DGControlModes {
    DGMouseDrag = 0,
    DGMouseFixed,
    DGMouseFree
};

enum DGConfigDefaults {
	DGDefAntialiasing = false,
	DGDefAudioBuffer = 8192,
    DGDefAudioDevice = 0,    
    DGDefAutopaths = true,    
    DGDefAutorun = true,
    DGDefBundleEnabled = true,
    DGDefControlMode = DGMouseFree,
	DGDefDisplayWidth = 1280,
	DGDefDisplayHeight = 800,
	DGDefDisplayDepth = 24,
	DGDefDebugMode = false,
    DGDefEffects = true,
	DGDefForcedFullScreen = false,
	DGDefFramebuffer = true,
	DGDefFramerate = 60,
	DGDefFullScreen = false,
	DGDefLog = true,
    DGDefMute = false,
    DGDefShowHelpers = false,
	DGDefShowSplash = true,
	DGDefShowSpots = false,
    DGDefSilentFeeds = false,
	DGDefSubtitles = true,
	DGDefTexCompression = false,
	DGDefVerticalSync = true
};

enum DGPaths {
	DGPathApp,
    DGPathRes,
    DGPathUser
};

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGConfig {
    char _appPath[DGMaxPathLength];
    char _resPath[DGMaxPathLength]; // Not to be confused with catalog
    char _scriptName[DGMaxFileLength];
	char _userPath[DGMaxPathLength];
    char _texExtension[4];
    
    int _fps;
    float _globalSpeed;
    
    // Private constructor/destructor
    DGConfig();
    ~DGConfig();
    // Stop the compiler generating methods of copy the object
    DGConfig(DGConfig const& copy);            // Not implemented
    DGConfig& operator=(DGConfig const& copy); // Not implemented
    
public:
    static DGConfig& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGConfig instance;
        return instance;
    }
    
    bool antialiasing;
    int audioBuffer;
    int audioDevice;
    bool autopaths;
    bool autorun;
    bool bundleEnabled;
    int controlMode;
    int displayWidth;
	int displayHeight;
	int displayDepth;
    bool debugMode;
    bool effects;
	bool forcedFullScreen;
    bool framebuffer;
	int framerate;
	bool fullScreen;
    bool log;
    bool mute;
    bool showHelpers;
    bool showSplash;
	bool showSpots;
    bool subtitles;
    bool silentFeeds;
    bool texCompression;
	bool verticalSync;
    
    float globalSpeed();
    
    int framesPerSecond();
    void setFramesPerSecond(int fps);
    
    const char* path(int ofType, const char* forFile, int andObject = DGObjectGeneric);
    void setPath(int forType, const char* path);
    
    const char*	script();
    void setScript(const char* name);    
    
    const char*	texExtension();
    void setTexExtension(const char* ext);
};

#endif // DG_CONFIG_H
