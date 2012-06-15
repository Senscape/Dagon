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

#include "DGCameraManager.h"
#include "DGConfig.h"
#include "DGConsole.h"
#include "DGCursorManager.h"
#include "DGLog.h"
#include "DGFontManager.h"
#include "DGRenderManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGConsole::DGConsole() {
    cameraManager = &DGCameraManager::getInstance();
    config = &DGConfig::getInstance();
    cursorManager = &DGCursorManager::getInstance();    
    fontManager = &DGFontManager::getInstance();
    log = &DGLog::getInstance();
    renderManager = &DGRenderManager::getInstance();
    
    _isEnabled = false; 
    _isReadyToProcess = false;;
    _size = DGConsoleRows * (DGDefFontSize + DGConsoleSpacing);
    _offset = _size;
    
    _state = DGConsoleHidden;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGConsole::~DGConsole() {
    // Nothing to do here
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGConsole::init() {
    _font = fontManager->loadDefault();
}

void DGConsole::deleteChar() {
    if (_command.size () > 0) 
        _command.resize(_command.size () - 1);
}

void DGConsole::disable() {
    _isEnabled = false;
}

void DGConsole::enable() {
    _isEnabled = true;
}

void DGConsole::execute() {
    log->command(DGModScript, _command.c_str());    
    _isReadyToProcess = true;
}

void DGConsole::getCommand(char* pointerToBuffer) {
    strncpy(pointerToBuffer, _command.c_str(), DGMaxLogLength);
    _command.clear();
    _isReadyToProcess = false;
}

void DGConsole::inputChar(char aKey) {
    if (_command.size() < DGMaxLogLength)
        _command.insert(_command.end(), aKey);
}

bool DGConsole::isEnabled() {
    return _isEnabled;
}

bool DGConsole::isHidden() {
    if (_state == DGConsoleHidden)
        return true;
    else
        return false;
} 

bool DGConsole::isReadyToProcess() {
    return _isReadyToProcess;
}

void DGConsole::toggle() {
    switch (_state) {
        case DGConsoleHidden:
        case DGConsoleHiding:
            _state = DGConsoleShowing;
            break;            
        case DGConsoleShowing:
        case DGConsoleVisible:
            _state = DGConsoleHiding;
            break;            
    }
}

void DGConsole::update() {
    DGPoint position = cursorManager->position();
    
    switch (_state) {
        case DGConsoleHidden:
            // Set the color used for information
            renderManager->setColor(DGColorBrightCyan);
            _font->print(DGInfoMargin, DGInfoMargin, 
                         "Viewport size: %d x %d", config->displayWidth, config->displayHeight);                
            _font->print(DGInfoMargin, (DGInfoMargin * 2) + DGDefFontSize, 
                         "Coordinates: (%d, %d)", (int)position.x, (int)position.y);
            _font->print(DGInfoMargin, (DGInfoMargin * 3) + (DGDefFontSize * 2), 
                         "Viewing angle: %2.1f", cameraManager->fieldOfView());
            _font->print(DGInfoMargin, (DGInfoMargin * 4) + (DGDefFontSize * 3), 
                         "FPS: %d", config->framesPerSecond()); 
            
            break;            
        case DGConsoleHiding:
            if (_offset < _size)
                _offset += DGConsoleSpeed;
            else 
                _state = DGConsoleHidden;
            break;
        case DGConsoleShowing:
            if (_offset > 0)
                _offset -= DGConsoleSpeed;
            else
                _state = DGConsoleVisible;
            break;
    }
    
    if (_state != DGConsoleHidden) {
        DGLogData logData;
        int row = (DGConsoleRows - 1); // Extra row saved for prompt
        
        float coords[] = { 0, -_offset, 
            config->displayWidth, -_offset,
            config->displayWidth, _size - _offset,
            0, _size - _offset };
        
        // Draw the slide
        renderManager->disableTextures();
        renderManager->setColor(0x25AA0000); // TODO: Add a mask color to achieve this (ie: ColoRed + Shade)
        renderManager->drawSlide(coords);
        renderManager->enableTextures();
        
        log->beginIteratingHistory();
        
        do {
            log->getCurrentLine(&logData);
            
            // Draw the current line
            renderManager->setColor(logData.color);
            _font->print(DGConsoleMargin, ((DGConsoleSpacing + DGDefFontSize) * row) - _offset, logData.line);
            
            row--;
        } while (log->iterateHistory());
        
        renderManager->setColor(DGColorBrightGreen);
        _font->print(DGConsoleMargin, (_size - (DGConsoleSpacing + DGDefFontSize)) - _offset, ">%s_", _command.c_str());
    }
}
