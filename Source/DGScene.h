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

#include "DGVideo.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class DGCameraManager;
class Config;
class DGCursorManager;
class DGRenderManager;
class Room;
class DGState;
class DGTexture;
class DGVideoManager;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

// TODO: Unify splash and cutscene codes
class DGScene {
    // References to singletons
    DGCameraManager& cameraManager;
    Config& config;
    DGCursorManager& cursorManager;
    DGRenderManager& renderManager;
    DGVideoManager& videoManager;
    
    // Other classes
    Room* _currentRoom;
    DGTexture* _cutsceneTexture;
    DGTexture* _splashTexture;
    
    bool _canDrawSpots = false; // This bool is used to make checks faster
    bool _isCutsceneLoaded = false;
    bool _isSplashLoaded = false;
    
public:
    DGScene();
    ~DGScene();
    
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

#endif // DG_SCENE_H
