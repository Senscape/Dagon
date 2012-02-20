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

#ifndef DG_FEEDMANAGER_H
#define DG_FEEDMANAGER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define DGDefFeedSize 10
#define DGFeedMargin 10
#define DGFeedSpeed 0.25f

enum DGFeedStates {
    DGFeedFadeIn,
    DGFeedIdle,
    DGFeedFadeOut,
    DGFeedDiscard
};

typedef struct {
	DGPoint location;
	uint32_t color;
    int state;
	char text[DGMaxFeedLength];
	int timerHandle;    
} DGFeed;

class DGConfig;
class DGTimerManager;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGFeedManager {
    DGConfig* config;
    DGTimerManager* timerManager;
    
    std::vector<DGFeed> _arrayOfFeeds;
    std::vector<DGFeed>::iterator it;
    
public:
    DGFeedManager();
    ~DGFeedManager();

    int color();
    DGPoint location();
    bool isQueued();
    void parse(const char* text);
    const char* text();
    void update();
};

#endif // DG_FEEDMANAGER_H
