////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2012 Senscape s.r.l.
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

#include "DGConfig.h"
#include "DGLog.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGLog::DGLog() {
    config = &DGConfig::getInstance();
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGLog::~DGLog() {
    if (_filestr.is_open())
        _filestr.close();
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void DGLog::_log(DGLogData* data) {
	if (config->log) {
        if (!_filestr.is_open())
            _filestr.open(config->path(DGPathUser, DGDefLogFile), fstream::app);
        
        time_t now = time(0);
        struct tm* tm = localtime(&now);
    
        _filestr << "[";
    
        switch(tm->tm_mon+1) {
            case 1: _filestr << "Jan "; break;
            case 2: _filestr << "Feb "; break;
            case 3: _filestr << "Mar "; break;
            case 4: _filestr << "Apr "; break;
            case 5: _filestr << "May "; break;
            case 6: _filestr << "Jun "; break;
            case 7: _filestr << "Jul "; break;
            case 8: _filestr << "Aug "; break;
            case 9: _filestr << "Sep "; break;
            case 10: _filestr << "Oct "; break;
            case 11: _filestr << "Nov "; break;
            case 12: _filestr << "Dec "; break;                
        }
    
        if (tm->tm_mday < 10)
            _filestr << " ";
    
		_filestr << tm->tm_mday << " " << tm->tm_hour << ":";
        
        if (tm->tm_min < 10)
            _filestr << "0";
        
        _filestr << tm->tm_min << ":";
        
        if (tm->tm_sec < 10)
            _filestr << "0";
        
        _filestr << tm->tm_sec << "] ";
        
		switch (data->module) {
			case DGModAudio:
				_filestr << "        AUDIO";
				break;
			case DGModCamera:
				_filestr << "       CAMERA";
				break;
			case DGModConfig:
				_filestr << "CONFIGURATION";
				break;					
			case DGModControl:
				_filestr << "   CONTROLLER";
				break;			
			case DGModCore:
				_filestr << "         CORE";
				break;
			case DGModEffects:
				_filestr << "      EFFECTS";
				break;
			case DGModFont:
				_filestr << "         FONT";
				break;	
			case DGModRender:
				_filestr << "       RENDER";
				break;
			case DGModScript:
				_filestr << "       SCRIPT";
				break;
			case DGModState:
				_filestr << "        STATE";
				break;
			case DGModSystem:
				_filestr << "       SYSTEM";
				break;				
			case DGModTexture:
				_filestr << "      TEXTURE";
				break;
			case DGModVideo:
				_filestr << "        VIDEO";
				break;
			case DGModNone:
			default:
				_filestr << "      GENERAL";
				break;
		}
    
        _filestr << " -> ";
    
        switch (data->type) {
            case DGLogError:
                _filestr << "ERROR: ";
                break;		
            case DGLogWarning:
                _filestr << "WARNING: ";
                break;
        }
    
        _filestr << data->line << "\n";
	}
	
    _filestr.flush();
    
    if (_history.size() == DGMaxLogHistory)
        _history.erase(_history.begin());
    
    _history.push_back(*data);
    
    if (config->debugMode) {
        // Echo to console
        printf("%s\n", data->line);
    }
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

DGLogData DGMakeLogData(int color, int module, int type)
{
	DGLogData logData;
	logData.color = color;
	logData.module = module;
	logData.type = type;

	return logData;
}

void DGLog::command(int forModule, const char* theString, ...) {
    DGLogData data;
	va_list ap;
	
	va_start(ap, theString);
	vsprintf(data.line, theString, ap);
	va_end(ap);
	
	data = DGMakeLogData(DGColorBrightMagenta, forModule, DGLogTrace);
	
	_log(&data);
}

void DGLog::error(int forModule, const char* theString, ...) {
    DGLogData data;
	va_list ap;
	
	va_start(ap, theString);
	vsprintf(data.line, theString, ap);
	va_end(ap);
	
	data = DGMakeLogData(DGColorBrightRed, forModule, DGLogError);
	
	_log(&data); 
}

void DGLog::info(int forModule, const char* theString, ...) {
    DGLogData data;
	va_list ap;
	
	va_start(ap, theString);
	vsprintf(data.line, theString, ap);
	va_end(ap);
	
	data = DGMakeLogData(DGColorBrightCyan, forModule, DGLogTrace);
	
	_log(&data);
}

void DGLog::trace(int forModule, const char* theString, ...) {
    DGLogData data;
	va_list ap;
	
	va_start(ap, theString);
	vsprintf(data.line, theString, ap);
	va_end(ap);
	
	data = DGMakeLogData(DGColorWhite, forModule, DGLogTrace);
	
	_log(&data); 
}

void DGLog::warning(int forModule, const char* theString, ...) {
    DGLogData data;
	va_list ap;
	
	va_start(ap, theString);
	vsprintf(data.line, theString, ap);
	va_end(ap);
	
	data = DGMakeLogData(DGColorYellow, forModule, DGLogWarning);
	
	_log(&data);    
}

////////////////////////////////////////////////////////////
// Implementation - Console helpers
////////////////////////////////////////////////////////////

bool DGLog::beginIteratingHistory() {
    if (!_history.empty()) {
        _it = _history.rbegin();
        return true;
    }
    
    return false;
}

bool DGLog::iterateHistory() {
    ++_it;
    
    if (_it == (_history.rend() - 1) || _history.empty()) // Bypass the first line  
        return false;
    else
        return true;
}

void DGLog::getCurrentLine(DGLogData* pointerToLogData) {
    // BUG: This is crashing in rare occasions, probably because the iterator is not set
    strcpy(pointerToLogData->line, _it->line);
    pointerToLogData->color = _it->color;
    pointerToLogData->module = _it->module;
    pointerToLogData->type = _it->type;
}
