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

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum ControlModes {
  kControlDrag = 0,
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
  kDefControlMode = kControlFixed,
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
  bool framebuffer;
  bool frameLimiter;
  int framerate;
  bool fullscreen;
  bool log;
  bool mute;
  bool showHelpers;
  bool showSplash;
  bool showSpots;
  bool silentFeeds;
  bool subtitles;
  bool texCompression;
  bool verticalSync;
  
  double framesPerSecond();
  float globalSpeed();
  std::string path(int ofType, const std::string &forFile, int andObject);
  std::string script();
  std::string texExtension();
  
  void setFramesPerSecond(double fps);
  void setPath(int forType, std::string thePath);
  void setScript(std::string name);
  void setTexExtension(std::string ext);
  
  // TODO: Provide members to save/load configurations
  // (also used by system to save them in the user's folder)
  
private:
  std::string _appPath;
  std::string _resPath;
  std::string _scriptName;
  std::string _userPath;
  std::string _texExtension;
  
  double _fps;
  double _globalSpeed;
  double _globalSpeedPrecision;
  double _targetGlobalSpeed;
  
  Config();
  ~Config() {};
  Config(const Config&);
  void operator=(const Config&);
};
  
}

#endif // DAGON_CONFIG_H_
