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

#define DGDefFontSize 10

class DGFont;
class DGLog;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGFontManager {
    DGLog* log;
    
    std::vector<DGFont*> _arrayOfFonts;
    
    DGFont _defaultFont;
    bool _isInitialized;
    FT_Library _library;
    
    // Private constructor/destructor
    DGFontManager();
    ~DGFontManager();
    // Stop the compiler generating methods of copy the object
    DGFontManager(DGFontManager const& copy);            // Not implemented
    DGFontManager& operator=(DGFontManager const& copy); // Not implemented
    
public:
    static DGFontManager& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGFontManager instance;
        return instance;
    }
    
    void init();
    DGFont* load(const char* fromFileName, unsigned int heightOfFont);
    DGFont* loadDefault();
};

#endif // DG_FONTMANAGER_H
