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

#ifndef DG_TEXTURE_H
#define DG_TEXTURE_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// TODO: We should convert all short vars to int for better speed here, also read new defines

typedef struct {
	//char	name[DGMaxFileLength];
    char	name[80];
	short	width;
	short	height;
	short	compressionLevel; // 0: None, 1: GL only, 2: GL & zlib
	short	numTextures;
} TEXMainHeader;

typedef struct {
	short	cubePosition;
	short	depth;
	int		size;
	int		format;
} TEXSubHeader;

class Config;
class DGLog;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGTexture : public DGObject {
    Config& config;
    DGLog& log;
    
    GLubyte* _bitmap;
    unsigned int _compressionLevel;
	GLuint _ident;
	GLint _width;
	GLint _height;
	GLint _depth;
    bool _hasResource;
    int _indexInBundle;
	bool _isBitmapLoaded;
	bool _isLoaded;
    
    std::mutex _mutex;    
    
    // This is used to keep track of the most used textures
    unsigned int _usageCount;
    
    // Eventually all file management will be handled by a DGResourceManager object
    char _resource[DGMaxFileLength];
    
public:
    DGTexture();
    DGTexture(int width, int height, int depth);
    ~DGTexture();
    
    // Checks

    bool hasResource();
    bool isLoaded();
    
    // Gets
    
    int depth();
    int indexInBundle();
    int height();
    const char* resource();
    unsigned int usageCount();
    int width();
    
    // Sets
    
    void increaseUsageCount();
    void setIndexInBundle(int index);
    void setResource(const char* fromFileName);
    
    // State changes
    
    void bind();
    void clear();
    void load();
    void loadBitmap(); // Temporary to test preloader
    
    // Textures loaded from memory are not managed
    void loadFromMemory(const unsigned char* dataToLoad, long size);
    void loadRawData(const unsigned char* dataToLoad, int width, int height);
    void saveToFile(const char* fileName);
    void unload();
};

#endif // DG_TEXTURE_H
