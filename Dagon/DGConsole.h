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

#ifndef DG_CONSOLE_H
#define DG_CONSOLE_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define DGConsoleMargin     10
#define DGConsoleRows       25
#define DGConsoleSpacing    6
#define DGConsoleSpeed      10

class DGConfig;
class DGFont;
class DGFontManager;
class DGLog;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGConsole {
    DGConfig* config;
    DGFontManager* fontManager;
    DGLog* log;
    
    DGFont* _font;
    
    std::string _command;
    bool _isEnabled;
    bool _isReadyToProcess;
    int _offset; // Used for sliding effect
    int _size;
    
    void _setColor(int color); // From the render module, to avoid an unnecessary dependency
    
public:
    DGConsole();
    ~DGConsole();
    
    void init();
    
    void disable();
    void enable();
    bool isHidden();
    bool isEnabled();
    void toggle();
    void update();
    
    void deleteChar();
    void execute();
    void getCommand(char* pointerToBuffer);
    void inputChar(char aKey);
    bool isReadyToProcess();
};

#endif // DG_CONSOLE_H
