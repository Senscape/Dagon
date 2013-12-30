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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "FontManager.h"
#include "Log.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

FontManager::FontManager() :
log(Log::instance())
{
  _isInitialized = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

FontManager::~FontManager() {
  // TODO: Must destroy all the fonts here
  
  if (_isInitialized) {
    _defaultFont.clear();
    FT_Done_FreeType(_library);
  }
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void FontManager::init(){
  log.trace(kModFont, "%s", kString15001);
  
  if (FT_Init_FreeType(&_library)) {
    log.error(kModFont, "%s", kString15003);
    return;
  }
  
  int major, minor, patch;
  
  FT_Library_Version(_library, &major, &minor, &patch);
  log.info(kModFont, "%s: %d.%d.%d", kString15002, major, minor, patch);
  
  setlocale(LC_CTYPE, "en_US.UTF-8");
  
  _isInitialized = true;
}

Font* FontManager::load(const char* fromFileName, unsigned int heightOfFont){
  Font* font = new Font;
  
  font->setLibrary(&_library);
  font->setResource(fromFileName, heightOfFont);
  
  _arrayOfFonts.push_back(font);
  
  return font;
}

Font* FontManager::loadDefault() {
  if (!_defaultFont.isLoaded()) {
    _defaultFont.setLibrary(&_library);
    _defaultFont.setDefault(kDefFontSize);
  }
  
  return &_defaultFont;
}
  
}
