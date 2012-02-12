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
// Interface
////////////////////////////////////////////////////////////

class DGLog {
    DGConfig* config;
    
    std::ofstream _filestr;
    std::vector<DGLogData> _history;
    std::vector<DGLogData>::iterator _it;
    void _log(DGLogData* data);
    
    // Private constructor/destructor
    DGLog();
    ~DGLog();
    // Stop the compiler generating methods of copy the object
    DGLog(DGLog const& copy);            // Not implemented
    DGLog& operator=(DGLog const& copy); // Not implemented
    
public:
    static DGLog& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGLog instance;
        return instance;
    }
    
    void command(int forModule, const char* theString, ...);
    void error(int forModule, const char* theString, ...); 
    void info(int forModule, const char* theString, ...);
    void trace(int forModule, const char* theString, ...);
    void warning(int forModule, const char* theString, ...);
    
    // Helpers for the console feature
    
    void beginIteratingHistory();
    bool iterateHistory();
    bool getCurrentLine(DGLogData* pointerToLogData);
};

#endif // DG_LOG_H
