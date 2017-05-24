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

#ifndef DAGON_SCENE_H_
#define DAGON_SCENE_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Video.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class CameraManager;
class Config;
class CursorManager;
class RenderManager;
class Room;
class Spot;
class State;
class Texture;
class VideoManager;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

// TODO: Unify splash and cutscene codes
class Scene {
  // References to singletons
  CameraManager& cameraManager;
  Config& config;
  CursorManager& cursorManager;
  RenderManager& renderManager;
  VideoManager& videoManager;
  
  // Other classes
  Room* _currentRoom;
  Texture* _cutsceneTexture;
  Texture* _splashTexture;

  Spot* _lastHoveredSpot;
  
  bool _canDrawSpots; // This bool is used to make checks faster
  bool _isCutsceneLoaded;
  bool _isSplashLoaded;
  
public:
  Scene();
  ~Scene();
  
  void clear();
  void drawSpots(bool disableVideos = false);
  void fadeIn();
  void fadeOut();
  bool scanSpots();
  void setRoom(Room* room);
  
  // Cutscene operations
  void cancelCutscene();
  bool drawCutscene();
  void loadCutscene(const char* fileName);
  void unloadCutscene();
  
  // Splash screen operations
  void drawSplash();
  void loadSplash();
  void unloadSplash();
};
  
}

#endif // DAGON_SCENE_H_
