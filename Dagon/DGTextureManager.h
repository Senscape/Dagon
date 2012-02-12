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

#ifndef DG_TEXTURE_MANAGER_H
#define DG_TEXTURE_MANAGER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"
#include "DGTexture.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

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
    void requestNewBundle(DGNode* forNode);
    void requestNewTexture(DGTexture* target);
    void requireTextureToLoad(DGTexture* target);
};

#endif // DG_TEXTURE_MANAGER_H
