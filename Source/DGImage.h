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
    DGConfig& config;
    
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
    
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setTexture(const char* fromFileName);

    // State changes
    
    void move(float offsetX, float offsetY);
    void scale(float factor);
};

#endif // DG_IMAGE_H
