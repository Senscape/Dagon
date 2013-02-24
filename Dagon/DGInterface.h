////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2012 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
//
////////////////////////////////////////////////////////////

#ifndef DG_INTERFACE_H
#define DG_INTERFACE_H

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class DGCameraManager;
class DGConfig;
class DGCursorManager;
class DGOverlay;
class DGRenderManager;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGInterface {
    // References to singletons
    DGCameraManager* cameraManager;    
    DGConfig* config;
    DGCursorManager* cursorManager;
    DGRenderManager* renderManager;
    
    std::vector<DGOverlay*> _arrayOfOverlays;
    std::vector<DGOverlay*> _arrayOfActiveOverlays; // Visible overlays go here
    
public:
    DGInterface();
    ~DGInterface();
    
    void addOverlay(DGOverlay* overlay);
    void drawCursor();
    void drawHelpers();
    void drawOverlays();
    void fadeIn();    
    void fadeOut();
    bool scanOverlays();
};

#endif // DG_INTERFACE_H
