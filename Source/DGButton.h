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

#ifndef DG_BUTTON_H
#define DG_BUTTON_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGAction.h"
#include "DGImage.h"
#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class Config;
class DGFont;
class DGFontManager;
class DGTexture;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGButton : public DGImage {
    Config& config;
    DGFontManager& fontManager;
    
    int _textColor;
    
    DGAction* _actionData;
    DGFont* _font;
    DGTexture* _attachedOnHoverTexture;
    std::string _text;
    
    bool _hasAction;
    bool _hasFont;
    bool _hasOnHoverTexture;    
    bool _hasText;
    
public:
    DGButton();
    ~DGButton();
    
    // Checks
    
    bool hasAction();
    bool hasFont();
    bool hasOnHoverTexture();    
    bool hasText();
    
    // Gets
    
    DGAction* action();
    DGFont* font();
    DGTexture* onHoverTexture();
    const char* text();
    int textColor();
    
    // Sets
    
    void setAction(DGAction* anAction);
    void setFont(const char* fromFileName, unsigned int heightOfFont);
    void setOnHoverTexture(const char* fromFileName);
    void setText(const char* text);
    void setTextColor(int aColor);
};

#endif // DG_BUTTON_H
