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

#ifndef DG_IMAGE_H
#define DG_IMAGE_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class DGConfig;
class DGTexture;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGImage : public DGObject {
    DGConfig* config;
    
    DGTexture* _attachedTexture;
    float _arrayOfCoordinates[8];
    bool _hasTexture;
    DGRect _rect;
    
    void _calculateCoordinates();
    
public:
    DGImage();
    DGImage(const char* fromFileName);
    ~DGImage();
    
    // Checks
    
    bool hasTexture();
      
    // Gets
    
    float* arrayOfCoordinates();
    DGPoint position();
    DGSize size();
    DGTexture* texture();

    // Sets
    
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setTexture(const char* fromFileName);

    // State changes
    
    void move(float offsetX, float offsetY);
    void scale(float factor);
};

#endif // DG_IMAGE_H
