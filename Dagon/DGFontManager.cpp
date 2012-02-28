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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGFontManager.h"
#include "DGLog.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGFontManager::DGFontManager() {
    log = &DGLog::getInstance();
    
    _isInitialized = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGFontManager::~DGFontManager() {
    if (_isInitialized) {
        _defaultFont.clear();
        FT_Done_FreeType(_library); 
    }
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGFontManager::init(){
    log->trace(DGModFont, "%s", DGMsg060000);
	log->info(DGModFont, "%s: %d.%d.%d", DGMsg060001, FREETYPE_MAJOR, FREETYPE_MINOR, FREETYPE_PATCH);
	
	if (FT_Init_FreeType(&_library)) {
		log->error(DGModFont, "%s", DGMsg260002);
        return;
    }
    
    _isInitialized = true;
}

DGFont* DGFontManager::load(const char* fromFileName, unsigned int heightOfFont){
    static DGFont font;
    
    font.setLibrary(&_library);
    font.setResource(fromFileName, heightOfFont);
    
    return &font;
}

DGFont* DGFontManager::loadDefault() {
    if (!_defaultFont.isLoaded()) {
        _defaultFont.setLibrary(&_library);
        _defaultFont.setDefault(DGDefFontSize);
    }
        
    return &_defaultFont;
}
