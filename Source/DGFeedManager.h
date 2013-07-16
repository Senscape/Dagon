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

#define DGFeedShadowEnabled     1
#define DGFeedShadowDistance    2

enum DGFeedStates {
    DGFeedFadeIn,
    DGFeedIdle,
    DGFeedFadeOut,
    DGFeedFadeOutSlow,
    DGFeedDiscard
};

typedef struct {
	DGPoint location;
	uint32_t color;
    int state;
	char text[DGMaxFeedLength];
    char audio[DGMaxFileLength];
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
    std::vector<DGFeed> _arrayOfActiveFeeds;
    std::vector<DGFeed> _arrayOfFeeds;    
    DGFont* _feedFont;
    int _feedHeight;
    
    void _calculatePosition(DGFeed* feed);
    void _dim();
    void _flush();
    
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

    void cancel();
    void clear(); // For clearing pending feeds
    void init();
    bool isPlaying();
    bool hasQueued();
    void queue(const char* text, const char* audio);
    void reshape();
    void setFont(const char* fromFileName, unsigned int heightOfFont);    
    void show(const char* text);
    void showAndPlay(const char* text, const char* audio);
    void update();
};

#endif // DG_FEEDMANAGER_H
