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

#ifndef DG_RENDERMANAGER_H
#define DG_RENDERMANAGER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <stdint.h>

#include "Platform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define kDefCursorDetail 30

class Config;
class EffectsManager;
class Log;
class Texture;

// Reference to embedded splash screen
extern "C" const unsigned char kSplashData[];

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class RenderManager {
  Config& config;
  EffectsManager& effectsManager;
  Log& log;
  
  GLuint _fbo; // The frame buffer object
  GLuint _fboDepth; // The depth buffer for the frame buffer object
  GLuint _fboTexture; // The texture object to write our frame buffer object to
  
  bool _blendNextUpdate;
  float _blendOpacity;
  GLfloat _defCursor[(kDefCursorDetail * 2) + 2];
  bool _alphaEnabled;
  float _helperLoop;
  
  bool _framebufferEnabled;
  bool _effectsEnabled;
  bool _fadeWithZoom;
  bool _texturesEnabled;
  
  Texture* _blendTexture;
  Texture* _fadeTexture;
  
  Point _centerOfPolygon(std::vector<int> arrayOfCoordinates); // Used for the helpers feature
  void _initFrameBuffer();
  void _initFrameBufferDepthBuffer();
  void _initFrameBufferTexture();
  
  std::vector<Point> _arrayOfHelpers;
  std::vector<Point>::iterator _itHelper;
  
  RenderManager();
  RenderManager(RenderManager const&);
  RenderManager& operator=(RenderManager const&);
  ~RenderManager();
  
public:
  static RenderManager& instance() {
    static RenderManager renderManager;
    return renderManager;
  }
  
  void init();
  
  // Control blend and fades
  
  void blendNextUpdate(bool fadeWithZoom = false);
  void fadeInNextUpdate();
  void fadeOutNextUpdate();
  void resetFade();
  
  // Conversion of coordinates (note this requires glu)
  
  Vector project(GLdouble x, GLdouble y, GLdouble z); // If more than three coordinates, attempts to calculate center
  Vector unProject(int x, int y);
  
  // Drawing operations
  
  void enableAlpha();
  void enablePostprocess();
  void enableTextures();
  void disableAlpha();
  void disablePostprocess();
  void disableTextures();
  void drawHelper(int xPosition, int yPosition, bool animate);
  void drawPolygon(std::vector<int> withArrayOfCoordinates, unsigned int onFace);
  void drawPostprocessedView(); // Expects orthogonal mode
  void drawSlide(float* withArrayOfCoordinates);
  void setAlpha(float alpha);
  void setColor(uint32_t color, float alpha = 0);
  uint32_t    testColor(int xPosition, int yPosition);
  
  // Helpers processing (indicates clickable spots)
  
  bool beginIteratingHelpers();
  Point currentHelper();
  bool iterateHelpers();
  
  // View operations, always used in the main loop
  
  void blendView();
  void clearView();
  void copyView();
  void fadeView();
  void resetView();
  void reshape();
};

#endif // DG_RENDERMANAGER_H
