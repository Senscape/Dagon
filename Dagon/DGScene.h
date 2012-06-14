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

#ifndef DG_SCENE_H
#define DG_SCENE_H

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class DGCamera;
class DGConfig;
class DGCursorManager;
class DGLog;
class DGOverlay;
class DGRender;
class DGRoom;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGScene {
    // References to singletons
    DGConfig* config;
    DGCursorManager* cursorManager;   
    DGLog* log;
    
    // Other classes
    DGCamera* _camera;
    DGRender* _render;
    DGRoom* _currentRoom;
    
    std::vector<DGOverlay*> _arrayOfOverlays;
    std::vector<DGOverlay*> _arrayOfActiveOverlays; // Visible overlays go here
    
    bool _canDrawSpots; // Used to make checks faster
    
public:
    DGScene();
    ~DGScene();
    
    void addOverlay(DGOverlay* overlay);
    void clear();
    void drawCursor();
    void drawHelpers();
    void drawOverlays();
    void drawSplash();
    void drawSpots();
    void fadeIn();    
    void fadeOut();
    void prepareSplash();
    bool scanOverlays();    
    bool scanSpots();
    void setRoom(DGRoom* room);
};

#endif // DG_SCENE_H
