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

#define DGDefCursorDetail 40

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
    
public:
    DGRender();
    ~DGRender();
    
    void beginDrawing(bool usingTextures);
    void blendNextUpdate();
    void drawCursor(int xPosition, int yPosition);
    void drawPolygon(std::vector<int> withArrayOfCoordinates, unsigned int onFace);
    
    // Slides always require four corners, so we know for sure the number of elements
    void drawSlide(int* withArrayOfCoordinates);    
    void endDrawing();
    void init();
    void setColor(int color);
    int	testColor(int xPosition, int yPosition);
    
    // Scene operations
    
    void clearScene();
    void copyScene();
    void updateScene();
};

#endif // DG_RENDER_H
