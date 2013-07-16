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

#ifndef DG_TEXTUREMANAGER_H
#define DG_TEXTUREMANAGER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"
#include "DGTexture.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// TODO: This class should be a singleton

// This is temporary until we actually test how much memory is available.
// NOT accurate! It's only a reference value since textures are flushed
// before the next switch.
#define DGMaxActiveTextures 6

class DGConfig;
class DGLog;
class DGNode;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGTextureManager {
    DGConfig* config;
    DGLog* log;
    
    std::vector<DGTexture*> _arrayOfActiveTextures;
    std::vector<DGTexture*> _arrayOfTextures;
    
public:
    DGTextureManager();
    ~DGTextureManager();
    
    void appendTextureToBundle(const char* nameOfBundle, DGTexture* textureToAppend);
    void createBundle(const char* nameOfBundle);
    int itemsInBundle(const char* nameOfBundle);
    void flush();
    void registerTexture(DGTexture* target);
    void requestBundle(DGNode* forNode);
    void requestTexture(DGTexture* target);
};

#endif // DG_TEXTUREMANAGER_H
