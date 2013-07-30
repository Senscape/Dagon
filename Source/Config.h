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

#ifndef DAGON_CONFIG_H_
#define DAGON_CONFIG_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <string>

#include "Defines.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum ControlModes {
  kControlDrag,
  kControlFixed,
  kControlFree
};

enum DefaultConfiguration {
  kDefAntialiasing = false,
  kDefAudioBuffer = 8192,
  kDefAudioDevice = 0,
  kDefAutopaths = true,
  kDefAutorun = true,
  kDefBundleEnabled = true,
  kDefControlMode = kControlFree,
  kDefDisplayWidth = 0,
  kDefDisplayHeight = 0,
  kDefDisplayDepth = 32,
  kDefDebugMode = true,
  kDefEffects = true,
  kDefFramebuffer = true,
  kDefFrameLimiter = false,
  kDefFramerate = 60,
  kDefFullscreen = false,
  kDefLog = true,
  kDefMute = false,
  kDefShowHelpers = false,
  kDefShowSplash = true,
  kDefShowSpots = false,
  kDefSilentFeeds = false,
  kDefSubtitles = true,
  kDefTexCompression = false,
  kDefVerticalSync = true
};

enum SystemPaths {
  kPathApp,
  kPathResources,
  kPathUserData
};

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class Config {
 public:
  static Config& instance() {
    static Config config;
    return config;
  }
    
  bool antialiasing = kDefAntialiasing;
  int audioBuffer = kDefAudioBuffer;
  int audioDevice = kDefAudioDevice;
  bool autopaths = kDefAutopaths;
  bool autorun = kDefAutorun;
  bool bundleEnabled = kDefBundleEnabled;
  int controlMode = kDefControlMode;
  int displayWidth = kDefDisplayWidth;
  int displayHeight = kDefDisplayHeight;
  int displayDepth = kDefDisplayDepth;
  bool debugMode = kDefDebugMode;
  bool effects = kDefEffects;
  bool framebuffer = kDefFramebuffer;
  bool frameLimiter = kDefFrameLimiter;
	int framerate = kDefFramerate;
	bool fullscreen = kDefFullscreen;
  bool log = kDefLog;
  bool mute = kDefMute;
  bool showHelpers = kDefShowHelpers;
  bool showSplash = kDefShowSplash;
	bool showSpots = kDefShowSpots;
  bool silentFeeds = kDefSilentFeeds;
  bool subtitles = kDefSubtitles;
  bool texCompression = kDefTexCompression;
	bool verticalSync = kDefVerticalSync;
  
  double framesPerSecond();
  double globalSpeed();
  std::string path(int ofType, const std::string &forFile, int andObject);
  std::string script();
  std::string texExtension();

  void setFramesPerSecond(double fps);
  void setPath(int forType, const std::string &path);
  void setScript(const std::string &name);
  void setTexExtension(const std::string &ext);
  
  // TODO: Provide members to save/load configurations
  // (also used by system to save them in the user's folder)
  
 private:
  std::string _appPath;
  std::string _resPath = kDefResourcePath;
  std::string _scriptName;
  std::string _userPath;
  std::string _texExtension = kDefTexExtension;
  
  double _fps = kDefFramerate;
  double _globalSpeed = 1.0;
  double _globalSpeedPrecision = kFrameratePrecision / 10.0;
  double _targetGlobalSpeed = 1.0;
  
  Config() {};
  ~Config() {};
  Config(const Config&);
  void operator=(const Config&);
};

#endif // DAGON_CONFIG_H_
