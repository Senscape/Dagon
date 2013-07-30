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
#define	DG_FONTMANAGER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "DGFont.h"
#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define kDefFontSize 10

class DGFont;
class DGLog;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGFontManager {
    DGLog& log;
    
    std::vector<DGFont*> _arrayOfFonts;
    
    DGFont _defaultFont;
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
    DGFont* load(const char* fromFileName, unsigned int heightOfFont);
    DGFont* loadDefault();
};

#endif // DG_FONTMANAGER_H
