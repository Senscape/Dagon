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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <sstream>

#include "Config.h"
#include "Font.h"
#include "Language.h"
#include "Log.h"

namespace dagon {

typedef int uint;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Font::Font() :
config(Config::instance()),
log(Log::instance())
{
  _isLoaded = false;
  this->setType(kObjectFont);
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void Font::clear() {
  if (_isLoaded)
    glDeleteTextures(kMaxChars, _textures);
}

bool Font::isLoaded() {
  return _isLoaded;
}

void Font::print(int x, int y, const char* text, ...) {
  if (_isLoaded) {
    char buffer[kMaxFeedLength];
    va_list ap;

    va_start(ap, text);
    int length = vsnprintf(buffer, kMaxFeedLength, text, ap);
    va_end(ap);

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(x, y, 0);

    for (const char* c = buffer; length > 0; c++, length--) {
      int ch = *c;

      GLfloat texCoords[] = { 0, 0, 0, _glyph[ch].y,
        _glyph[ch].x, _glyph[ch].y, _glyph[ch].x, 0 };

      GLshort coords[] = { 0, 0, 0, _glyph[ch].rows,
        _glyph[ch].width, _glyph[ch].rows, _glyph[ch].width, 0 };

      glBindTexture(GL_TEXTURE_2D, _textures[ch]);
      glTranslatef(static_cast<GLfloat>(_glyph[ch].left), 0, 0);
      glPushMatrix();
      glTranslatef(0, -static_cast<GLfloat>(_glyph[ch].top) + _height, 0);
      glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
      glVertexPointer(2, GL_SHORT, 0, coords);
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      glPopMatrix();
      glTranslatef(static_cast<GLfloat>(_glyph[ch].advance >> 6), 0, 0);
    }
    glPopMatrix();
    glPopAttrib();
  }
}

void Font::wPrint(int x, int y, const char* text) {
  if (_isLoaded) {
    size_t origsize = strlen(text) + 1;
    wchar_t wcstring[kMaxFeedLength];
   // setlocale(LC_CTYPE, "en_US.UTF-8");
    mbstowcs(wcstring, text, origsize);
   // setlocale(LC_ALL,"C");

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(x, y, 0);

    size_t length = wcslen(wcstring);
    for (const wchar_t* c = wcstring; length > 0; c++, length--) {
      unsigned long ch = *c;
      GLfloat texCoords[] = { 0, 0, 0, _glyph[ch].y,
        _glyph[ch].x, _glyph[ch].y, _glyph[ch].x, 0 };

      GLshort coords[] = { 0, 0, 0, _glyph[ch].rows,
        _glyph[ch].width, _glyph[ch].rows, _glyph[ch].width, 0 };

      glBindTexture(GL_TEXTURE_2D, _textures[ch]);
      glTranslatef(static_cast<GLfloat>(_glyph[ch].left), 0, 0);
      glPushMatrix();
      glTranslatef(0, -static_cast<GLfloat>(_glyph[ch].top) + _height, 0);
      glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
      glVertexPointer(2, GL_SHORT, 0, coords);
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      glPopMatrix();
      glTranslatef(static_cast<GLfloat>(_glyph[ch].advance >> 6), 0, 0);
    }
    glPopMatrix();
    glPopAttrib();
  }
}

// FIXME: This is a repeated method from RenderManager --
//  it would be best to avoid this.
void Font::setColor(uint32_t color) {
  uint8_t b = (color & 0x000000ff);
  uint8_t g = (color & 0x0000ff00) >> 8;
  uint8_t r = (color & 0x00ff0000) >> 16;
  uint8_t a = (color & 0xff000000) >> 24;

  glColor4f(r/255.0f, g/255.0f, b/255.0f, a/255.0f);
}

void Font::setDefault(unsigned int heightOfFont) {
  FT_Face face;

  if (FT_New_Memory_Face(*_library, kFontData, kSizeOfFontData, 0, &face)) {
    log.error(kModFont, "%s", kString15005);
    return;
  }

  _height = heightOfFont;
  _loadFont(face);
  FT_Done_Face(face);
}

void Font::setLibrary(FT_Library* library) {
  _library = library;
}

void Font::setResource(const std::string &fromFileName,
                       unsigned int heightOfFont) {
  FT_Face face;

  if (FT_New_Face(*_library,
                  config.path(kPathResources, fromFileName,
                              kObjectFont).c_str(),
                  0, &face)) {
    log.error(kModFont, "%s: %s", kString15004, fromFileName.c_str());
    return;
  }

  _height = heightOfFont;
  _loadFont(face);
  FT_Done_Face(face);
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void Font::_loadFont(FT_Face &face) {
  FT_Set_Char_Size(face, _height << 6, _height << 6, 96, 96);
  glGenTextures(kMaxChars, _textures);

  for (wchar_t ch = 0; ch < kMaxChars; ch++) {
    if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT)) {
      log.error(kModFont, "%s: %c", kString15006, ch);
      return;
    }

    FT_Glyph glyph;
    if (FT_Get_Glyph(face->glyph, &glyph)) {
      log.error(kModFont, "%s: %c", kString15007, ch);
      return;
    }

    FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
    FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;
    FT_Bitmap bitmap = bitmapGlyph->bitmap;

    uint width = _next(bitmap.width);
    uint height = _next(bitmap.rows);
    GLubyte* expandedData;
    expandedData = new GLubyte[2 * width * height];

    for (uint j = 0; j < height; j++) {
      for (uint i = 0; i < width; i++) {
        expandedData[2 * (i + j * width)] =
        expandedData[2 * (i + j * width) + 1] =
        (i >= bitmap.width || j >= bitmap.rows) ?
        0 : bitmap.buffer[i + bitmap.width * j];
      }
    }

    float x = static_cast<float>(bitmap.width) / static_cast<float>(width);
    float y = static_cast<float>(bitmap.rows) / static_cast<float>(height);
    _glyph[ch] = _makeGlyph(x, y, bitmap, bitmapGlyph, face);

    glBindTexture(GL_TEXTURE_2D, _textures[ch]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 2, width, height, 0,
                 GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expandedData);
    delete[] expandedData;
  }
  _isLoaded = true;
}

Glyph Font::_makeGlyph(float x, float y, FT_Bitmap bitmap,
                       FT_BitmapGlyph bitmapGlyph, FT_Face face) {
  Glyph glyph;
  glyph.x = x;
  glyph.y = y;
  glyph.width = bitmap.width;
  glyph.rows = bitmap.rows;
  glyph.left = bitmapGlyph->left;
  glyph.top = bitmapGlyph->top;
  glyph.advance = face->glyph->advance.x;
  return glyph;
}

int Font::_next(int a) {
  int rval = 1;
  while (rval < a) rval <<= 1;
  return rval;
}

}
