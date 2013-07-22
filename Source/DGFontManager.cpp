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

#include "DGFontManager.h"
#include "DGLog.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGFontManager::DGFontManager() :
    log(DGLog::instance())
{
    _isInitialized = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGFontManager::~DGFontManager() {
    // TODO: Must destroy all the fonts here
    
    if (_isInitialized) {
        _defaultFont.clear();
        FT_Done_FreeType(_library); 
    }
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGFontManager::init(){
    log.trace(DGModFont, "%s", DGMsg060000);
	
	if (FT_Init_FreeType(&_library)) {
		log.error(DGModFont, "%s", DGMsg260002);
        return;
    }
    
    int major, minor, patch;
    
    FT_Library_Version(_library, &major, &minor, &patch);
    log.info(DGModFont, "%s: %d.%d.%d", DGMsg060001, major, minor, patch);
    
    _isInitialized = true;
}

DGFont* DGFontManager::load(const char* fromFileName, unsigned int heightOfFont){
    DGFont* font = new DGFont;
    
    font->setLibrary(&_library);
    font->setResource(fromFileName, heightOfFont);
    
    _arrayOfFonts.push_back(font);
    
    return font;
}

DGFont* DGFontManager::loadDefault() {
    if (!_defaultFont.isLoaded()) {
        _defaultFont.setLibrary(&_library);
        _defaultFont.setDefault(DGDefFontSize);
    }
        
    return &_defaultFont;
}
