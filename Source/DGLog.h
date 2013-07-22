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

#ifndef DG_LOG_H
#define DG_LOG_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum DGLogModules {
	DGModAudio,
	DGModCamera,
	DGModConfig,
	DGModControl,
	DGModCore,
	DGModEffects,
	DGModFont,
	DGModRender,
	DGModScript,
	DGModState,
	DGModSystem,
	DGModTexture,
	DGModVideo,
	DGModNone
};

enum DGLogTypes {
	DGLogTrace,
	DGLogError,
	DGLogWarning
};

typedef struct {
	char line[DGMaxLogLength];
    int color;
    int module;
    int type;
} DGLogData;

class DGConfig;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGLog {
    DGConfig& config;
    
    std::ofstream _filestr;
    std::vector<DGLogData> _history;
    std::vector<DGLogData>::reverse_iterator _it;
    void _log(DGLogData* data);
    
    DGLog();
    DGLog(DGLog const&);
    DGLog& operator=(DGLog const&);
    ~DGLog();
    
public:
    static DGLog& instance() {
        static DGLog log;
        return log;
        
    }
    
    void command(int forModule, const char* theString, ...);
    void error(int forModule, const char* theString, ...); 
    void info(int forModule, const char* theString, ...);
    void trace(int forModule, const char* theString, ...);
    void warning(int forModule, const char* theString, ...);
    
    // Helpers for the console feature
    
    bool beginIteratingHistory();
    bool iterateHistory();
    void getCurrentLine(DGLogData* pointerToLogData);
};

#endif // DG_LOG_H
