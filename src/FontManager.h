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

#ifndef DAGON_FONTMANAGER_H_
#define DAGON_FONTMANAGER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "Font.h"
#include "Platform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define kDefFontSize 10

namespace dagon {

class Font;
class Log;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class FontManager {
  Log& log;
  
  std::vector<Font*> _arrayOfFonts;
  
  Font _defaultFont;
  bool _isInitialized;
  FT_Library _library;
  
  FontManager();
  FontManager(FontManager const&);
  FontManager& operator=(FontManager const&);
  ~FontManager();
  
public:
  static FontManager& instance() {
    static FontManager fontManager;
    return fontManager;
  }
  
  void init();
  Font* load(const char* fromFileName, unsigned int heightOfFont);
  Font* loadDefault();
};
  
}

#endif // DAGON_FONTMANAGER_H_
