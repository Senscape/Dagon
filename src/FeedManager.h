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

#ifndef DAGON_FEEDMANAGER_H_
#define DAGON_FEEDMANAGER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Platform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define kDefFeedSize 10
#define DGFeedMargin 25
#define DGFeedMaxLines 10
#define DGFeedSpeed 0.25

#define DGFeedShadowEnabled     1
#define DGFeedShadowDistance    2

namespace dagon {

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
class AudioManager;
class Config;
class Font;
class FontManager;
class TimerManager;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class FeedManager {
  AudioManager& audioManager;
  Config& config;
  FontManager& fontManager;
  TimerManager& timerManager;
  
  Audio* _feedAudio;
  std::vector<DGFeed> _arrayOfActiveFeeds;
  std::vector<DGFeed> _arrayOfFeeds;
  Font* _feedFont;
  unsigned int _feedHeight;
  
  void _calculatePosition(DGFeed* feed);
  void _dim();
  void _flush();
  
  FeedManager();
  FeedManager(FeedManager const&);
  FeedManager& operator=(FeedManager const&);
  ~FeedManager();
  
public:
  static FeedManager& instance() {
    static FeedManager feedManager;
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
  
}

#endif // DAGON_FEEDMANAGER_H_
