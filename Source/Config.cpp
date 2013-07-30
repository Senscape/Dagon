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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <assert.h>

#include "Config.h"
#include "DGObject.h"

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

double Config::globalSpeed() {
  if (_globalSpeed < _targetGlobalSpeed) {
      _globalSpeed += _globalSpeedPrecision;
  } else if (_globalSpeed > _targetGlobalSpeed) {
      _globalSpeed -= _globalSpeedPrecision;
  }
    
  return _globalSpeed;
}

double Config::framesPerSecond() {
  return _fps;
}

void Config::setFramesPerSecond(double fps) {
  _targetGlobalSpeed = kDefFramerate / _fps;
  _fps = fps;
}

std::string Config::path(int ofType, const std::string &forFile,
                         int andObject) {
  std::string fullPath;
	
  switch (ofType) {
    case kPathApp: {
      fullPath = _appPath;
      if (autopaths) {
        if (andObject == DGObjectRoom)
          fullPath += kDefRoomPath;
      }
      break;
    }
    case kPathResources: {
      fullPath = _resPath;
      if (autopaths) {
        switch (andObject) {
          case DGObjectAudio: {
            fullPath += kDefAudioPath;
            break;
          }
          case DGObjectCursor: {
            fullPath += kDefCursorPath;
            break;
          }
          case DGObjectFont: {
            fullPath += kDefFontPath;
            break;
          }
          case DGObjectImage: {
            fullPath += kDefImagePath;
            break;
          }
          case DGObjectNode: {
            fullPath += kDefNodePath;
            break;
          }
          case DGObjectVideo: {
            fullPath += kDefVideoPath;
            break;
          }
          default: {
            assert(false);
          }
        }
      }
      break;
    }
    case kPathUserData: {
      fullPath = _userPath;
      break;
    }
  }

  fullPath += forFile;
	
	return fullPath;   
}

void Config::setPath(int forType, const std::string &path) {
  switch (forType) {
    case kPathApp: {
      _appPath = path;
      break;
    }
    case kPathResources: {
      _resPath = path;
      break;
    }
    case kPathUserData: {
      _userPath = path;
      break;
    }
    default: {
      assert(false);
    }
  }
}

std::string Config::script() {
  return _scriptName;
}

void Config::setScript(const std::string &name) {
  _scriptName = name;
}

std::string	Config::texExtension() {
  return _texExtension;
}

void Config::setTexExtension(const std::string &ext) {
  _texExtension = ext;
  
  if (_texExtension == "tex") {
    bundleEnabled = true;
  } else {
    bundleEnabled = false;
  }
}
