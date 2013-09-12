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

#ifndef DG_FONTMANAGER_H
#define DG_FONTMANAGER_H

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

class Font;
class Log;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGFontManager {
    Log& log;
    
    std::vector<Font*> _arrayOfFonts;
    
    Font _defaultFont;
    bool _isInitialized;
    FT_Library _library;
    
    DGFontManager();
    DGFontManager(DGFontManager const&);
    DGFontManager& operator=(DGFontManager const&);
    ~DGFontManager();
    
public:
    static DGFontManager& instance() {
        static DGFontManager fontManager;
        return fontManager;
    }
    
    void init();
    Font* load(const char* fromFileName, unsigned int heightOfFont);
    Font* loadDefault();
};

#endif // DG_FONTMANAGER_H
