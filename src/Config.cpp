////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <cassert>

#include "Config.h"
#include "Object.h"
#include "Script.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Config::Config() {
  antialiasing = kDefAntialiasing;
  audioBuffer = kDefAudioBuffer;
  audioDevice = kDefAudioDevice;
  autopaths = kDefAutopaths;
  autorun = kDefAutorun;
  bundleEnabled = kDefBundleEnabled;
  controlMode = kDefControlMode;
  displayWidth = kDefDisplayWidth;
  displayHeight = kDefDisplayHeight;
  displayDepth = kDefDisplayDepth;
  debugMode = kDefDebugMode;
  effects = kDefEffects;
  framebuffer = kDefFramebuffer;
  frameLimiter = kDefFrameLimiter;
  framerate = kDefFramerate;
  fullscreen = kDefFullscreen;
  log = kDefLog;
  mute = kDefMute;
  numOfAudioBuffers = kDefNumOfAudioBuffers;
  showHelpers = kDefShowHelpers;
  showSplash = kDefShowSplash;
  showSpots = kDefShowSpots;
  silentFeeds = kDefSilentFeeds;
  subtitles = kDefSubtitles;
  texCompression = kDefTexCompression;
  verticalSync = kDefVerticalSync;
  _scriptName = kDefScriptFile;
  _resPath = kDefResourcePath;
  _texExtension = kDefTexExtension;
  
  _fps = kDefFramerate;
  _globalSpeed = 1.0;
  _globalSpeedPrecision = kFrameratePrecision / kDefFramerate;
  _targetGlobalSpeed = 1.0;
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

double Config::framesPerSecond() {
  return _fps;
}

float Config::globalSpeed() {
  if (_globalSpeed < _targetGlobalSpeed) {
    _globalSpeed += _globalSpeedPrecision;
  } else if (_globalSpeed > _targetGlobalSpeed) {
    _globalSpeed -= _globalSpeedPrecision;
  }
  
  return static_cast<float>(_globalSpeed);
}

std::string Config::path(int ofType, const std::string &forFile,
                         int andObject) {
  std::string fullPath;
  
  switch (ofType) {
    case kPathApp: {
      fullPath = _appPath;
      if (autopaths) {
        if (andObject == kObjectRoom) {
          fullPath += "scripts/";
          fullPath += kDefRoomPath;
        }
      }
      break;
    }
    case kPathResources: {
      fullPath = _resPath;
      if (autopaths) {
        // This is a temporary hack until the Resources manager is implemented
        if (Script::instance().isExecutingModule()) {
          std::string roomPath = Script::instance().module();
          fullPath += "rooms/";
          fullPath += roomPath;
          fullPath += "/";
        }
        else {
          switch (andObject) {
            case kObjectAudio: {
              fullPath += kDefAudioPath;
              break;
            }
            case kObjectCursor: {
              fullPath += kDefCursorPath;
              break;
            }
            case kObjectFont: {
              fullPath += kDefFontPath;
              break;
            }
            case kObjectImage: {
              fullPath += kDefImagePath;
              break;
            }
            case kObjectNode: {
              fullPath += kDefNodePath;
              break;
            }
            case kObjectVideo: {
              fullPath += kDefVideoPath;
              break;
            }
            default: {
              assert(false);
            }
          }
        }
      }
      break;
    }
    case kPathUserData: {
      fullPath = _userPath;
      if (andObject == kObjectSave)
        fullPath += kDefSavePath;
      break;
    }
    default: {
      assert(false);
    }
  }
  
  fullPath += forFile;
  
  return fullPath;
}

std::string Config::script() {
  return _scriptName;
}

std::string Config::texExtension() {
  return _texExtension;
}

void Config::setFramesPerSecond(double fps) {
  _targetGlobalSpeed = kDefFramerate / _fps;
  _fps = fps;
}

void Config::setPath(int forType, std::string thePath) {
  switch (forType) {
    case kPathApp: {
      _appPath = thePath;
      break;
    }
    case kPathResources: {
      _resPath = thePath;
      break;
    }
    case kPathUserData: {
      _userPath = thePath;
      break;
    }
    default: {
      assert(false);
    }
  }
}

void Config::setScript(std::string name) {
  _scriptName = name;
}

void Config::setTexExtension(std::string ext) {
  _texExtension = ext;
  
  if (_texExtension == "tex") {
    bundleEnabled = true;
  } else {
    bundleEnabled = false;
  }
}
  
}
