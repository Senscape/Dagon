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
#define DGInfoMargin        10

enum DGConsoleStates {
    DGConsoleHidden,
    DGConsoleHiding,
    DGConsoleShowing,
    DGConsoleVisible
};

class DGCameraManager;
class DGConfig;
class DGCursorManager;
class DGFont;
class DGFontManager;
class DGLog;
class DGRenderManager;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGConsole {
    DGCameraManager& cameraManager;
    DGConfig& config;
    DGCursorManager& cursorManager;
    DGFontManager& fontManager;
    DGLog& log;
    DGRenderManager& renderManager;
    
    DGFont* _font;
    
    std::string _command;
    bool _isEnabled;
    bool _isInitialized;
    bool _isReadyToProcess;
    int _offset; // Used for sliding effect
    int _size;
    int _state;
    
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
