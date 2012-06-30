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

// Reference to embedded splash screen
extern const unsigned char DGDefSplashBinary[];

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGRenderManager {
    DGConfig* config;
    DGLog* log;
    
    bool _blendNextUpdate;
    float _blendOpacity;
    GLfloat _defCursor[(DGDefCursorDetail * 2) + 2];
    bool _alphaEnabled;
    bool _shadersEnabled;
    bool _texturesEnabled;
    
    DGPoint _centerOfPolygon(std::vector<int> arrayOfCoordinates); // Used for the helpers feature
    DGTexture* _blendTexture;
    DGTexture* _fadeTexture;   
    
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
    
    void blendNextUpdate();
    void fadeInNextUpdate();
    void fadeOutNextUpdate();
    void resetFade();
    
    // Conversion of coordinates (note this requires glu)
    
    DGVector project(float x, float y, float z); // If more than three coordinates, attempts to calculate center
    DGVector unProject(int x, int y);

    // Drawing operations
    
    void enableAlpha();
    void enableTextures();
    void disableAlpha();    
    void disableTextures();
    void drawHelper(int xPosition, int yPosition, bool animate);
    void drawPolygon(std::vector<int> withArrayOfCoordinates, unsigned int onFace);
    void drawSlide(float* withArrayOfCoordinates); // We use float in all "slides" since we need the precision
    void setAlpha(float alpha);
    void setColor(int color);
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
};

#endif // DG_RENDER_H
