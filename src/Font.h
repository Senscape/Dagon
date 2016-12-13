////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DAGON_FONT_H_
#define DAGON_FONT_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <string>
#include <stdint.h>
#include <wchar.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <GL/glew.h>

#include "Object.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////

class Config;
class Log;

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

const wchar_t kMaxChars = 256;

// This structure holds information from the Freetype font
typedef struct {
  float x;
  float y;
  GLshort width;
  GLshort rows;
  int left;
  int top;
  long advance;
} Glyph;

// When default font is selected, we use data embedded in the
// executable and declared in FontData.cpp
extern "C" const unsigned char kFontData[];
extern "C" const long kSizeOfFontData;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class Font : public Object {
 public:
  Font();
  ~Font() {};
  
  void clear();
  bool isLoaded();
  void print(int x, int y, const char* text, ...);
  void wPrint(int x, int y, const char* text);
  void setColor(uint32_t color);
  void setDefault(unsigned int heightOfFont);
  void setLibrary(FT_Library* library);
  void setResource(const std::string &fromFileName, unsigned int heightOfFont);
  
 private:
  Config& config;
  Log& log;
  
  Glyph _glyph[kMaxChars];
  unsigned int _height;
  bool _isLoaded;
  FT_Library* _library;
  GLuint _textures[kMaxChars];
  
  void _loadFont(FT_Face &face);
  Glyph _makeGlyph(float x, float y, FT_Bitmap bitmap,
                   FT_BitmapGlyph bitmapGlyph, FT_Face face);
  int _next(int a);
  
  Font(const Font&);
  void operator=(const Font&);
};
  
}

#endif // DAGON_FONT_H_
