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

#ifndef DG_FONT_H
#define	DG_FONT_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_STROKER_H

#include <GL/glew.h>
#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// This structure holds information from the Freetype font
typedef struct {
	float x;
	float y;
	int width;
	int rows;
	int left;
	int top;
	int advance;
} DGGlyph;
 
// When default font is selected, we use data embedded in the
// executable and declared in DGFontData.c
extern const unsigned char DGDefFontBinary[];

class DGConfig;
class DGLog;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGFont : public DGObject {
    DGConfig* config;
    DGLog* log;
    
    FT_Face _face;
    DGGlyph _glyph[128];
    int _height;
    bool _isLoaded;
    FT_Library* _library;
	GLuint* _textures;
    
    void _loadFont();
    int _next(int a);
    
public:
    DGFont();
    ~DGFont();

    void clear();
    bool isLoaded();
    void print(int x, int y, const char* text, ...);
    void setColor(int color);
    void setDefault(unsigned int heightOfFont);
    void setLibrary(FT_Library* library);
    void setResource(const char* fromFileName, unsigned int heightOfFont);
};

#endif // DG_FONT_H
