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

#ifndef DG_SCENE_H
#define DG_SCENE_H

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
  
  bool _canDrawSpots; // This bool is used to make checks faster
  bool _isCutsceneLoaded;
  bool _isSplashLoaded;
  
public:
  Scene();
  ~Scene();
  
  void clear();
  void drawSpots();
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

#endif // DG_SCENE_H
