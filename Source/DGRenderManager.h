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

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define DGDefCursorDetail 30

class DGConfig;
class DGEffectsManager;
class DGLog;
class DGTexture;

// Reference to embedded splash screen
extern "C" const unsigned char DGDefSplashBinary[];

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGRenderManager {
    DGConfig* config;
    DGEffectsManager* effectsManager;    
    DGLog* log;
    
    GLuint _fbo; // The frame buffer object  
    GLuint _fboDepth; // The depth buffer for the frame buffer object  
    GLuint _fboTexture; // The texture object to write our frame buffer object to 
    
    bool _blendNextUpdate;
    float _blendOpacity;
    GLfloat _defCursor[(DGDefCursorDetail * 2) + 2];
    bool _alphaEnabled;
    float _helperLoop;
    
    bool _framebufferEnabled;
    bool _effectsEnabled;
    bool _fadeWithZoom;
    bool _texturesEnabled;
    
    DGTexture* _blendTexture;
    DGTexture* _fadeTexture;   
    
    DGPoint _centerOfPolygon(std::vector<int> arrayOfCoordinates); // Used for the helpers feature
    void _initFrameBuffer();
    void _initFrameBufferDepthBuffer();
    void _initFrameBufferTexture();
    
    std::vector<DGPoint> _arrayOfHelpers;
    std::vector<DGPoint>::iterator _itHelper;
    
    // Private constructor/destructor
    DGRenderManager();
    ~DGRenderManager();
    // Stop the compiler generating methods of copy the object
    DGRenderManager(DGRenderManager const& copy);            // Not implemented
    DGRenderManager& operator=(DGRenderManager const& copy); // Not implemented
    
public:
    static DGRenderManager& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGRenderManager instance;
        return instance;
    }
    
    void init();
    
    // Control blend and fades
    
    void blendNextUpdate(bool fadeWithZoom = false);
    void fadeInNextUpdate();
    void fadeOutNextUpdate();
    void resetFade();
    
    // Conversion of coordinates (note this requires glu)
    
    DGVector project(float x, float y, float z); // If more than three coordinates, attempts to calculate center
    DGVector unProject(int x, int y);

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
    void drawSlide(int* withArrayOfCoordinates);
    void setAlpha(float alpha);
    void setColor(int color, float alpha = 0);
    int	testColor(int xPosition, int yPosition);
    
    // Helpers processing (indicates clickable spots)
    
    bool beginIteratingHelpers();
    DGPoint currentHelper();
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
