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
    int _arrayOfCoordinates[8];
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
    
    int* arrayOfCoordinates();
    DGPoint position();
    DGSize size();
    DGTexture* texture();

    // Sets
    
    void setPosition(int x, int y);
    void setSize(int width, int height);
    void setTexture(const char* fromFileName);    

    // State changes
    
    void move(int offsetX, int offsetY);
    void scale(float factor);
};

#endif // DG_IMAGE_H
