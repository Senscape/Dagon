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

#include "Config.h"
#include "EffectsManager.h"
#include "Log.h"
#include "RenderManager.h"
#include "Texture.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

RenderManager::RenderManager() :
config(Config::instance()),
effectsManager(EffectsManager::instance()),
log(Log::instance())
{
  _blendTexture = NULL;
  _fadeTexture = NULL;
  _fadeWithZoom = false;
  _helperLoop = 0.0f;
  
  _blendNextUpdate = false;
  _texturesEnabled = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

RenderManager::~RenderManager() {
  delete _blendTexture;
  delete _fadeTexture;
}

////////////////////////////////////////////////////////////
// Implementation - Init sequence
////////////////////////////////////////////////////////////

void RenderManager::init() {
  glewInit();
  
  const GLubyte* version = glewGetString(GLEW_VERSION);
  log.info(kModRender, "%s: %s", kString11005, version);
  
  version = glGetString(GL_VERSION);
  log.trace(kModRender, "%s", kString11001);
  log.info(kModRender, "%s: %s", kString11002, version);
  
  if (glewIsSupported("GL_VERSION_2_0")) {
    _effectsEnabled = true;
    effectsManager.init();
  }
  else {
    log.warning(kModRender, "%s", kString11003);
    _effectsEnabled = false;
  }
  
  _alphaEnabled = true;
  
  // WARNING: This next setting could make things slower
  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glDisable(GL_DITHER);
  
  glDisable(GL_DEPTH_TEST);
  //glDepthFunc(GL_ALWAYS);
  //glDepthMask(GL_TRUE);
  
  if (config.antialiasing) {
    // FIXME: Some of these options may be introducing black lines
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    
    glEnable(GL_MULTISAMPLE); // Not sure if this one goes here
  }
  
  glClearDepth(1.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  
  _blendTexture = new Texture(0, 0, 0); // All default values
  _fadeTexture = new Texture(1, 1, 0); // Minimal black texture
  _fadeTexture->setFadeSpeed(kFadeFastest);
  
  // NOTE: Here we read the default screen values to calculate the aspect ratio
  for (int i = 0; i < (kDefCursorDetail + 1) * 2; i += 2) {
    _defCursor[i] = static_cast<GLfloat>((.0075 * cos(i * 1.87 * M_PI / kDefCursorDetail)) * config.displayWidth);
    _defCursor[i + 1] = static_cast<GLfloat>((.01 * sin(i * 1.87 * M_PI / kDefCursorDetail)) * config.displayHeight);
  }
  
  glEnableClientState(GL_VERTEX_ARRAY);
  
  if (config.framebuffer)
    _initFrameBuffer();
}

////////////////////////////////////////////////////////////
// Implementation - Blend and fades
////////////////////////////////////////////////////////////

void RenderManager::blendNextUpdate(bool fadeWithZoom) {
  _blendOpacity = 0.0f;
  _blendTexture->bind();
  this->copyView();
  _blendNextUpdate = true;
  _fadeWithZoom = fadeWithZoom;
}

void RenderManager::fadeInNextUpdate() {
  _fadeTexture->fadeOut();
}

void RenderManager::fadeOutNextUpdate() {
  _fadeTexture->fadeIn();
}

void RenderManager::resetFade() {
  _fadeTexture->setFadeLevel(0.0f);
}

////////////////////////////////////////////////////////////
// Implementation - Conversion of coordinates
////////////////////////////////////////////////////////////

Vector RenderManager::project(GLdouble x, GLdouble y, GLdouble z) {
  Vector vector;
  GLdouble winX, winY, winZ;
  
  // Arrays to hold matrix information
  
  GLdouble modelView[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
  
  GLdouble projection[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  
  // Get window coordinates based on the 3D object
  
  gluProject(x, y, z,
             modelView, projection, viewport,
             &winX, &winY, &winZ);
  
  vector.x = winX;
  vector.y = config.displayHeight - winY; // This one must be inverted
  vector.z = winZ;
  
  return vector;
}

Vector RenderManager::unProject(int x, int y) {
  Vector vector;
  GLdouble objX, objY, objZ;
  
  // Arrays to hold matrix information
  
  GLdouble modelView[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
  
  GLdouble projection[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  
  // Get window coordinates based on the 3D object
  
  gluUnProject((GLdouble)x, GLdouble(y), 0.0f,
               modelView, projection, viewport,
               &objX, &objY, &objZ);
  
  vector.x = objX;
  vector.y = objY;
  vector.z = objZ;
  
  return vector;
}

////////////////////////////////////////////////////////////
// Implementation - Drawing operations
////////////////////////////////////////////////////////////

void RenderManager::enableAlpha() {
  _alphaEnabled = true;
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderManager::enablePostprocess() {
  if (_framebufferEnabled)
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo); // Bind our frame buffer for rendering
}

void RenderManager::enableTextures() {
  if (!_texturesEnabled) {
    _texturesEnabled = true;
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
  }
}

void RenderManager::disableAlpha() {
  _alphaEnabled = false;
  glBlendFunc(GL_ONE, GL_ZERO);
}

void RenderManager::disablePostprocess() {
  effectsManager.drawDust();
  
  if (_framebufferEnabled)
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // Unbind our texture
}

void RenderManager::disableTextures() {
  if (_texturesEnabled) {
    _texturesEnabled = false;
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }
}

void RenderManager::drawHelper(int xPosition, int yPosition, bool animate) {
  glDisable(GL_LINE_SMOOTH);
  
  // TODO: Test later if push & pop is necessary at this point
  glPushMatrix();
  glTranslatef(xPosition, yPosition, 0);
  
  if (animate) {
    GLfloat currColor[4];
    glGetFloatv(GL_CURRENT_COLOR, currColor);
    glColor4f(currColor[0], currColor[1], currColor[2], 1.0f - _helperLoop);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glScalef(1.0f * (_helperLoop * 2.0f), 1.0f * (_helperLoop * 2.0f), 0);
  }
  else {
    glScalef(1.0f, 1.1f, 0);
    glBlendFunc(GL_ONE, GL_ONE);
  }
  
  glVertexPointer(2, GL_FLOAT, 0, _defCursor);
  
  glDrawArrays(GL_LINE_LOOP, 0, (kDefCursorDetail >> 1) + 2);
  
  if (animate) glScalef(0.85f * _helperLoop, 0.85f * _helperLoop, 0);
  else glScalef(0.835f, 0.85f, 0);
  glDrawArrays(GL_TRIANGLE_FAN, 0, (kDefCursorDetail >> 1) + 2);
  glPopMatrix();
  
  glEnable(GL_LINE_SMOOTH);
  
  if (_alphaEnabled)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  else
    glBlendFunc(GL_ONE, GL_ZERO);
}

void RenderManager::drawPolygon(std::vector<int> withArrayOfCoordinates, unsigned int onFace) {
  const float x = 1.0f;
  const float y = 1.0f;
  
  int i, j;
  int cubeTextureSize = kDefTexSize;
  int sizeOfArray = (int)withArrayOfCoordinates.size();
  int numCoords = sizeOfArray + (sizeOfArray >> 1);
  GLfloat* spotVertCoords = new GLfloat[numCoords];
  
  glPushMatrix();
  
  switch (onFace) {
    case kNorth:
      glTranslatef(-x, y, -x);
      for (i = 0, j = 0; i < sizeOfArray; i += 2, j += 3) {
        // Size is divided in half because of the way we draw the cube
        spotVertCoords[j] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize >> 1);
        // We must invert the coordinates in some cases
        spotVertCoords[j + 1] = (GLfloat)withArrayOfCoordinates[i + 1] / (GLfloat)(cubeTextureSize >> 1) * -1;
        spotVertCoords[j + 2] = 0.0f;
      }
      break;
    case kEast:
      glTranslatef(x, y, -x);
      for (i = 0, j = 0; i < sizeOfArray; i += 2, j += 3) {
        spotVertCoords[j] = 0.0f;
        spotVertCoords[j + 1] = (GLfloat)withArrayOfCoordinates[i + 1] / (GLfloat)(cubeTextureSize >> 1) * -1;
        spotVertCoords[j + 2] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize >> 1);
      }
      break;
    case kSouth:
      glTranslatef(x, y, x);
      for (i = 0, j = 0; i < sizeOfArray; i += 2, j += 3) {
        spotVertCoords[j] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize >> 1) * -1;
        spotVertCoords[j + 1] = (GLfloat)withArrayOfCoordinates[i + 1] / (GLfloat)(cubeTextureSize >> 1) * -1;
        spotVertCoords[j + 2] = 0.0f;
      }
      break;
    case kWest:
      glTranslatef(-x, y, x);
      for (i = 0, j = 0; i < sizeOfArray; i += 2, j += 3) {
        spotVertCoords[j] = 0.0f;
        spotVertCoords[j + 1] = (GLfloat)withArrayOfCoordinates[i + 1] / (GLfloat)(cubeTextureSize >> 1) * -1;
        spotVertCoords[j + 2] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize >> 1) * -1;
      }
      break;
    case kUp:
      glTranslatef(-x, y, x);
      for (i = 0, j = 0; i < sizeOfArray; i += 2, j += 3) {
        spotVertCoords[j] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize >> 1);
        spotVertCoords[j + 1] = 0.0f;
        spotVertCoords[j + 2] = (GLfloat)withArrayOfCoordinates[i + 1] / (GLfloat)(cubeTextureSize >> 1) * -1;
      }
      break;
    case kDown:
      glTranslatef(-x, -y, -x);
      for (i = 0, j = 0; i < sizeOfArray; i += 2, j += 3) {
        spotVertCoords[j] = (GLfloat)withArrayOfCoordinates[i] / (GLfloat)(cubeTextureSize >> 1);
        spotVertCoords[j + 1] = 0.0f;
        spotVertCoords[j + 2] = (GLfloat)withArrayOfCoordinates[i + 1] / (GLfloat)(cubeTextureSize >> 1);
      }
      break;
  }
  
  if (_texturesEnabled) {
    float texU = (float)1 / (cubeTextureSize * 2);
    float texV = (float)((cubeTextureSize * 2) - 1) / (cubeTextureSize * 2);
    
    GLfloat texCoords[] = {texU, texU, texV, texU, texV, texV, texU, texV};
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
  }
  else {
    // We can safely assume this spot has a color and therefore can use
    // a "helper". Not the most elegant way to do this but, hey, it works.
    Point center = _centerOfPolygon(withArrayOfCoordinates);
    
    // This code is a bit redundant but optimal: the center is only calculated in
    // the required cases. Basically, this projects the calculated point onto the
    // corresponding face.
    float cx, cy, cz;
    
    switch (onFace) {
      case kNorth:
        cx = (GLfloat)center.x / (GLfloat)(cubeTextureSize >> 1);
        cy = (GLfloat)center.y / (GLfloat)(cubeTextureSize >> 1) * -1;
        cz = 0.0f;
        break;
      case kEast:
        cx = 0.0f;
        cy = (GLfloat)center.y / (GLfloat)(cubeTextureSize >> 1) * -1;
        cz = (GLfloat)center.x / (GLfloat)(cubeTextureSize >> 1);
        break;
      case kSouth:
        cx = (GLfloat)center.x / (GLfloat)(cubeTextureSize >> 1) * -1;
        cy = (GLfloat)center.y / (GLfloat)(cubeTextureSize >> 1) * -1;
        cz = 0.0f;
        break;
      case kWest:
        cx = 0.0f;
        cy = (GLfloat)center.y / (GLfloat)(cubeTextureSize >> 1) * -1;
        cz = (GLfloat)center.x / (GLfloat)(cubeTextureSize >> 1) * -1;
        break;
      case kUp:
        cx = (GLfloat)center.x / (GLfloat)(cubeTextureSize >> 1);
        cy = 0.0f;
        cz = (GLfloat)center.y / (GLfloat)(cubeTextureSize >> 1) * -1;
        break;
      case kDown:
        cx = (GLfloat)center.x / (GLfloat)(cubeTextureSize >> 1);
        cy = 0.0f;
        cz = (GLfloat)center.y / (GLfloat)(cubeTextureSize >> 1);
        break;
    }
    
    Vector vector = this->project(cx, cy, cz);
    
    if (vector.z < 1.0) { // Only store coordinates on screen
      Point point;
      
      point.x = static_cast<int>(vector.x);
      point.y = static_cast<int>(vector.y);
      
      _arrayOfHelpers.push_back(point);
    }
  }
  
  glVertexPointer(3, GL_FLOAT, 0, spotVertCoords);
  glDrawArrays(GL_TRIANGLE_FAN, 0, sizeOfArray >> 1);
  
  delete spotVertCoords;
  
  glPopMatrix();
}

void RenderManager::drawPostprocessedView() {
  if (_framebufferEnabled) {
    glBindTexture(GL_TEXTURE_2D, _fboTexture); // Bind our frame buffer texture
    
    if (config.effects) {
      effectsManager.play();
      effectsManager.update();
    }
    
    float coords[] = {0, config.displayHeight,
      config.displayWidth, config.displayHeight,
      config.displayWidth, 0,
      0, 0};
    
    this->drawSlide(coords);
    
    effectsManager.pause();
    
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind any textures
  }
}

void RenderManager::drawSlide(float* withArrayOfCoordinates) {
  glPushMatrix();
  
  if (_texturesEnabled) {
    GLfloat texCoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
  }
  
  glVertexPointer(2, GL_FLOAT, 0, withArrayOfCoordinates);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  
  glPopMatrix();
}

void RenderManager::setAlpha(float alpha) {
  // NOTE: This resets the current color so it should be used with care
  glColor4f(1.0f, 1.0f, 1.0f, alpha);
}

void RenderManager::setColor(uint32_t color, float alpha) {
  uint8_t b = (color & 0x000000ff);
  uint8_t g = (color & 0x0000ff00) >> 8;
  uint8_t r = (color & 0x00ff0000) >> 16;
  uint8_t a = (color & 0xff000000) >> 24;
  
  if (fabs(alpha) > kEpsilon)
    glColor4f(r/255.0f, g/255.0f, b/255.0f, alpha); // Force specified alpha
  else
    glColor4f(r/255.0f, g/255.0f, b/255.0f, a/255.f);
}

// FIXME: glReadPixels has an important performace hit on older computers. Improve.
uint32_t    RenderManager::testColor(int xPosition, int yPosition) {
  // This is static because it sometimes throws a stack corruption error
  static GLubyte pixel[3];
  GLint r, g, b;
  uint32_t aux;
  
  // Note we flip the Y coordinate here because only the orthogonal projection is flipped
  glReadPixels(xPosition, config.displayHeight - yPosition, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
  
  r = (GLint)pixel[0];
  g = (GLint)pixel[1];
  b = (GLint)pixel[2];
  
  aux = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
  
  return aux;
}

////////////////////////////////////////////////////////////
// Implementation - Helpers processing
////////////////////////////////////////////////////////////

bool RenderManager::beginIteratingHelpers() {
  if (!_arrayOfHelpers.empty()) {
    if (_helperLoop > 1.0f) _helperLoop = 0.0f;
    else _helperLoop += 0.015f; // This is an arbitrary speed
    
    _itHelper = _arrayOfHelpers.begin();
    
    return true;
  }
  
  return false;
}

Point RenderManager::currentHelper() {
  return *_itHelper;
}

bool RenderManager::iterateHelpers() {
  ++_itHelper;
  
  if (_itHelper == _arrayOfHelpers.end())
    return false;
  else
    return true;
}

////////////////////////////////////////////////////////////
// Implementation - View operations
////////////////////////////////////////////////////////////

void RenderManager::clearView() {
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void RenderManager::copyView() {
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, config.displayWidth, config.displayHeight, 0);
}

void RenderManager::blendView() {
  if (_blendNextUpdate) {
    int xStretch;
    int yStretch;
    
    if (_fadeWithZoom) {
      xStretch = static_cast<int>(_blendOpacity) * (config.displayWidth >> 2);
      yStretch = static_cast<int>(_blendOpacity) * (config.displayHeight >> 2);
      
      // This should a bit faster than the walk_time factor
      _blendOpacity += 0.015f * config.globalSpeed();
    }
    else {
      xStretch = 0;
      yStretch = 0;
      _blendOpacity += 0.0125f * config.globalSpeed();
    }
    
    // Note the coordinates here are inverted because of the way the screen is captured
    float coords[] = {-xStretch, config.displayHeight + yStretch,
      config.displayWidth + xStretch, config.displayHeight + yStretch,
      config.displayWidth + xStretch, -yStretch,
      -xStretch, -yStretch};
    
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f - _blendOpacity);
    
    _blendTexture->bind();
    this->drawSlide(coords);
    
    if (_blendNextUpdate) {
      if (_blendOpacity >= 1.0f) {
        _blendOpacity = 0.0f;
        _blendNextUpdate = false;
      }
    }
  }
}

void RenderManager::resetView() {
  // Check for errors in loop
  if (config.debugMode) {
    GLenum errCode;
    const GLubyte *errString;
    
    if ((errCode = glGetError()) != GL_NO_ERROR) {
      errString = gluErrorString(errCode);
      log.error(kModRender, "%s: %s", kString11006, errString);
    }
  }
  
  glLoadIdentity();
  _arrayOfHelpers.clear();
}

void RenderManager::reshape() {
  glBindTexture(GL_TEXTURE_2D, _fboTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, config.displayWidth, config.displayHeight, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}

void RenderManager::fadeView() {
  // FIXME: This is done every time and it sucks
  
  float coords[] = {0, 0,
    config.displayWidth, 0,
    config.displayWidth, config.displayHeight,
    0, config.displayHeight};
  
  _fadeTexture->updateFade();
  _fadeTexture->bind();
  this->setAlpha(_fadeTexture->fadeLevel());
  this->drawSlide(coords);
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

Point RenderManager::_centerOfPolygon(std::vector<int> arrayOfCoordinates) {
  Point center;
  int size = static_cast<int>(arrayOfCoordinates.size());
  int vertex = size >> 1;
  
  double area = 0.0;
  double x0 = 0.0; // Current vertex X
  double y0 = 0.0; // Current vertex Y
  double x1 = 0.0; // Next vertex X
  double y1 = 0.0; // Next vertex Y
  double a = 0.0; // Partial signed area
  
  center.x = 0;
  center.y = 0;
  
  // For all vertices
  for (int i = 0; i < vertex; ++i) {
    x0 = arrayOfCoordinates[i << 1];
    y0 = arrayOfCoordinates[(i << 1) + 1];
    x1 = arrayOfCoordinates[((i << 1) + 2) % size];
    y1 = arrayOfCoordinates[((i << 1) + 3) % size];
    
    a = (x0 * y1) - (x1 * y0);
    area += a;
    
    center.x += (x0 + x1) * a;
    center.y += (y0 + y1) * a;
  }
  
  area *= 3.0;
  double invArea = 1.0 / area;
  center.x *= invArea;
  center.y *= invArea;
  
  return center;
}

void RenderManager::_initFrameBuffer() {
  // _initFrameBufferDepthBuffer(); // Initialize our frame buffer depth buffer
  
  _initFrameBufferTexture(); // Initialize our frame buffer texture
  
  glGenFramebuffersEXT(1, &_fbo); // Generate one frame buffer and store the ID in fbo
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo); // Bind our frame buffer
  
  // Attach the texture fbo_texture to the color buffer in our frame buffer
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, _fboTexture, 0);
  
  // Attach the depth buffer fbo_depth to our frame buffer
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _fboDepth);
  
  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); // Check that status of our generated frame buffer
  
  if (status != GL_FRAMEBUFFER_COMPLETE_EXT) { // If the frame buffer does not report back as complete
    log.warning(kModRender, "%s", kString11004);
    _framebufferEnabled = false;
  }
  else _framebufferEnabled = true;
  
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // Unbind our texture
}

// Obsolete
void RenderManager::_initFrameBufferDepthBuffer() {
  glGenRenderbuffersEXT(1, &_fboDepth); // Generate one render buffer and store the ID in fbo_depth
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _fboDepth); // Bind the fbo_depth render buffer
  
  // Set the render buffer storage to be a depth component, with a width and height of the window
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
                           config.displayWidth, config.displayHeight);
  
  // Set the render buffer of this buffer to the depth buffer
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                               GL_RENDERBUFFER_EXT, _fboDepth);
  
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0); // Unbind the render buffer
}

void RenderManager::_initFrameBufferTexture() {
  glGenTextures(1, &_fboTexture); // Generate one texture
  glBindTexture(GL_TEXTURE_2D, _fboTexture); // Bind the texture fbo_texture
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, config.displayWidth, config.displayHeight, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, NULL); // Create a standard texture with the width and height of our window
  
  // Setup the basic texture parameters
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  // Unbind the texture
  glBindTexture(GL_TEXTURE_2D, 0);
}
  
}
