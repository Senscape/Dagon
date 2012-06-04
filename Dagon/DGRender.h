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

#ifndef DG_RENDER_H
#define DG_RENDER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define DGDefCursorDetail 30

class DGConfig;
class DGLog;
class DGTexture;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGRender {
    DGConfig* config;
    DGLog* log;
    
    bool _blendNextUpdate;
    GLfloat _defCursor[(DGDefCursorDetail * 2) + 2];
    bool _shadersEnabled;
    bool _texturesEnabled;
    
    DGTexture* _blendTexture;
    
    std::vector<DGPoint> _arrayOfHelpers;
    std::vector<DGPoint>::iterator _itHelper;
    
    DGPoint _centerOfPolygon(std::vector<int> arrayOfCoordinates);
    
public:
    DGRender();
    ~DGRender();
    
    void beginDrawing(bool usingTextures);
    void blendNextUpdate();
    void drawHelper(int xPosition, int yPosition, bool animate);
    void drawPolygon(std::vector<int> withArrayOfCoordinates, unsigned int onFace);
    
    // Slides always require four corners, so we know for sure the number of elements
    void drawSlide(float* withArrayOfCoordinates); // We use float in all "slides" since we need the precision
    void endDrawing();
    void init();
    void setAlpha(float alpha);
    void setColor(int color);
    int	testColor(int xPosition, int yPosition);
    
    // Scene operations
    
    void clearScene();
    void copyScene();
    void resetScene();    
    void updateScene();
    
    // Conversion of coordinates (note this requires glu)
    
    DGVector project(float x, float y, float z); // If more than three coordinates, attempts to calculate center
    DGVector unProject(int x, int y);
    
    // Process available helpers (indicates clickable spots)
    bool beginIteratingHelpers();
    DGPoint currentHelper();
    bool iterateHelpers();
};

#endif // DG_RENDER_H
