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

#ifndef DG_TEXTURE_H
#define DG_TEXTURE_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include "DGConfig.h"
#include "DGDefines.h"
#include "DGLog.h"
#include "DGObject.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// TODO: We should convert all short vars to int

typedef struct {
	char	name[DGMaxFileLength];
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

typedef struct {
    char	file[DGMaxFileLength];
    int     index;
} DGResource;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGTexture : public DGObject {
    DGConfig* config;
    DGLog* log;
    
    GLubyte* _bitmap;
    unsigned int _compressionLevel;
	GLuint _ident;
	GLint _width;
	GLint _height;
	GLint _depth;
    bool _hasResource;
    int _indexInBundle;
	bool _isLoaded;
    
    // This is used to keep trace of the most used textures
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
    void load();
    
    // Textures loaded from memory are not managed
    void loadFromMemory(const unsigned char* dataToLoad);
    void saveToFile(const char* fileName);
    void unload();
};

#endif // DG_TEXTURE_H
