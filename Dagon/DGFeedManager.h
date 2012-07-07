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
#define DGFeedMargin 25
#define DGFeedMaxLines 5
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

class DGAudio;
class DGAudioManager;
class DGConfig;
class DGFont;
class DGFontManager;
class DGTimerManager;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGFeedManager {
    DGAudioManager* audioManager;    
    DGConfig* config;
    DGFontManager* fontManager;
    DGTimerManager* timerManager;
    
    DGAudio* _feedAudio;
    std::vector<DGFeed> _arrayOfFeeds;
    DGFont* _feedFont;
    int _feedHeight;
    
    // Private constructor/destructor
    DGFeedManager();
    ~DGFeedManager();
    // Stop the compiler generating methods of copy the object
    DGFeedManager(DGFeedManager const& copy);            // Not implemented
    DGFeedManager& operator=(DGFeedManager const& copy); // Not implemented
    
public:
    static DGFeedManager& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGFeedManager instance;
        return instance;
    }

    void init();
    bool isQueued();
    void parse(const char* text);
    void parseWithAudio(const char* text, const char* audio);
    void setFont(const char* fromFileName, unsigned int heightOfFont);
    void update();
};

#endif // DG_FEEDMANAGER_H
