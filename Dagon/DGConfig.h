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

#include "DGDefines.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum DGConfigDefaults {
	DGDefAntialiasing = true,
    DGDefAutorun = true,
    DGDefBundleEnabled = true, // Not exported to Lua
	DGDefDisplayWidth = 1024,
	DGDefDisplayHeight = 768,
	DGDefDisplayDepth = 32,
	DGDefDebugMode = true, // This shouldn't default to true, careful while finding paths
    DGDefDust = false,  // Move to effects
    DGDefEffects = true,
	DGDefFramerate = 60,
	DGDefFullScreen = false,
	DGDefLog = true,
    DGDefScare = false, // Move to effects
	DGDefShowSplash = true,
	DGDefShowSpots = false,
	DGDefTexCompression = true,
	DGDefThrob = false,  // Move to effects    
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
    bool autorun;
    bool bundleEnabled;
    int displayWidth;
	int displayHeight;
	int displayDepth;
    bool debugMode;
    bool dust;
    bool effects;
	int framerate;
	bool fullScreen;
    bool log;
	bool texCompression;
    bool scare;
    bool showSplash;
	bool showSpots;
    bool throb;
	bool verticalSync;
    
    float globalSpeed();
    
    const char* path(int ofType, const char* forFile);
    void setPath(int forType, const char* path);
    
    const char*	script();
    void setScript(const char* name);    
    
    const char*	texExtension();
    void setTexExtension(const char* ext);
};

#endif // DG_CONFIG_H
