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

#include "Platform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define kDefFeedSize 10
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
	Point location;
	uint32_t color;
    int state;
	char text[kMaxFeedLength];
    char audio[kMaxFileLength];
	int timerHandle;    
} DGFeed;

class Audio;
class DGAudioManager;
class Config;
class Font;
class DGFontManager;
class DGTimerManager;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class DGFeedManager {
    DGAudioManager& audioManager;
    Config& config;
    DGFontManager& fontManager;
    DGTimerManager& timerManager;
    
    Audio* _feedAudio;
    std::vector<DGFeed> _arrayOfActiveFeeds;
    std::vector<DGFeed> _arrayOfFeeds;    
    Font* _feedFont;
    int _feedHeight;
    
    void _calculatePosition(DGFeed* feed);
    void _dim();
    void _flush();
    
    DGFeedManager();
    DGFeedManager(DGFeedManager const&);
    DGFeedManager& operator=(DGFeedManager const&);
    ~DGFeedManager();
    
public:
    static DGFeedManager& instance() {
        static DGFeedManager feedManager;
        return feedManager;
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
