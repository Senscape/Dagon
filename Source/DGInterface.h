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
class Config;
class DGCursorManager;
class Overlay;
class DGRenderManager;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGInterface {
  // References to singletons
  DGCameraManager& cameraManager;
  Config& config;
  DGCursorManager& cursorManager;
  DGRenderManager& renderManager;
  
  std::vector<Overlay*> _arrayOfOverlays;
  std::vector<Overlay*> _arrayOfActiveOverlays; // Visible overlays go here
  
public:
  DGInterface();
  ~DGInterface();
  
  void addOverlay(Overlay* overlay);
  void drawCursor();
  void drawHelpers();
  void drawOverlays();
  void fadeIn();
  void fadeOut();
  bool scanOverlays();
};

#endif // DG_INTERFACE_H
