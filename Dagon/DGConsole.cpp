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

#include <GL/glew.h>
#include "DGConfig.h"
#include "DGConsole.h"
#include "DGLog.h"
#include "DGFontManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGConsole::DGConsole() {
    config = &DGConfig::getInstance();
    fontManager = &DGFontManager::getInstance();
    log = &DGLog::getInstance();
    
    _isEnabled = false;
    _isReadyToProcess = false;
    _size = DGConsoleRows * (DGDefFontSize + DGConsoleSpacing);
    _offset = _size;    
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

void DGConsole::disable() {
    _isEnabled = false;
}

void DGConsole::enable() {
    _isEnabled = true;
}

bool DGConsole::isHidden() {
    if (_offset >= _size)
        return true;
    else
        return false;
} 

bool DGConsole::isEnabled() {
    return _isEnabled;
}

void DGConsole::toggle() {
    _isEnabled = !_isEnabled;
}

void DGConsole::update() {
    DGLogData logData;
    int row = (DGConsoleRows - 1); // Extra row saved for prompt
    
    int coords[] = { 0, -_offset, 
        config->displayWidth, -_offset,
        config->displayWidth, _size - _offset,
        0, _size - _offset };
    
    // Draw the slide
	glEnableClientState(GL_VERTEX_ARRAY);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    _setColor(0x25AA0000); // TODO: Add a mask color to achieve this (ie: ColoRed + Shade)
    glPushMatrix();
    glVertexPointer(2, GL_INT, 0, coords);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glPopMatrix();
    
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
    
    log->beginIteratingHistory();
    
    do {
        log->getCurrentLine(&logData);
        
        // Draw the current line
        _setColor(logData.color);
        _font->print(DGConsoleMargin, ((DGConsoleSpacing + DGDefFontSize) * row) - _offset, logData.line);
        
        row--;
    } while (log->iterateHistory());
    
    _setColor(DGColorBrightGreen);
    _font->print(DGConsoleMargin, (_size - (DGConsoleSpacing + DGDefFontSize)) - _offset, ">%s_", _command.c_str());
    
    if (_isEnabled) {
        if (_offset > 0)
            _offset -= DGConsoleSpeed;
    }
    else {
        if (_offset < _size)
            _offset += DGConsoleSpeed;
    }
    
    glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void DGConsole::deleteChar() {
    if (_command.size () > 0) 
        _command.resize(_command.size () - 1);
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

bool DGConsole::isReadyToProcess() {
    return _isReadyToProcess;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void DGConsole::_setColor(int color) {
   	uint32_t aux = color;
    
	uint8_t b = (aux & 0x000000ff);
	uint8_t g = (aux & 0x0000ff00) >> 8;
	uint8_t r = (aux & 0x00ff0000) >> 16;
	uint8_t a = (aux & 0xff000000) >> 24;
    
	glColor4f((float)(r / 255.0f), (float)(g / 255.0f), (float)(b / 255.0f), (float)(a / 255.f)); 
}
