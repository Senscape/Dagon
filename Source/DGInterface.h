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
    DGCameraManager& cameraManager;
    DGConfig& config;
    DGCursorManager& cursorManager;
    DGRenderManager& renderManager;
    
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
