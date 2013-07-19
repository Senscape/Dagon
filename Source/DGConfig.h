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
	DGDefDebugMode = true,
    DGDefEffects = true,
	DGDefForcedFullScreen = false,
	DGDefFramebuffer = true,
    DGDefFrameLimiter = false,
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
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGConfig {
    char _appPath[DGMaxPathLength];
    char _resPath[DGMaxPathLength];
    char _scriptName[DGMaxFileLength];
	char _userPath[DGMaxPathLength];
    char _texExtension[4];
    
    double _fps;
    double _globalSpeed;
    
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
    bool frameLimiter;
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
    
    double globalSpeed();
    
    double framesPerSecond();
    void setFramesPerSecond(double fps);
    
    const char* path(int ofType, const char* forFile, int andObject = DGObjectGeneric);
    void setPath(int forType, const char* path);
    
    const char*	script();
    void setScript(const char* name);    
    
    const char*	texExtension();
    void setTexExtension(const char* ext);
};

#endif // DG_CONFIG_H
