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
class DGConfig;
class DGCursorManager;
class DGRenderManager;
class DGRoom;
class DGTexture;
class DGVideoManager;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

// TODO: Unify splash and cutscene codes
class DGScene {
    // References to singletons
    DGCameraManager& cameraManager;
    DGConfig& config;
    DGCursorManager& cursorManager;
    DGRenderManager& renderManager;
    DGVideoManager& videoManager;
    
    // Other classes
    DGRoom* _currentRoom;
    DGTexture* _cutsceneTexture;
    DGTexture* _splashTexture;
    
    bool _canDrawSpots; // This bool is used to make checks faster
    bool _isCutsceneLoaded;
    bool _isSplashLoaded;
    
public:
    DGScene();
    ~DGScene();
    
    void clear();
    void drawSpots();
    void fadeIn();    
    void fadeOut(); 
    bool scanSpots();
    void setRoom(DGRoom* room);
    
    // Cutscene operations
    bool drawCutscene();
    void loadCutscene(const char* fileName);
    void unloadCutscene();
    
    // Splash screen operations
    void drawSplash();
    void loadSplash();
    void unloadSplash();
};

#endif // DG_SCENE_H
