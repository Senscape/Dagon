////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "AudioManager.h"
#include "Config.h"
#include "Control.h"
#include "FeedManager.h"
#include "FontManager.h"
#include "Room.h"
#include "TimerManager.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

FeedManager::FeedManager() :
audioManager(AudioManager::instance()),
config(Config::instance()),
fontManager(FontManager::instance()),
timerManager(TimerManager::instance())
{
  _feedHeight = kDefFeedSize;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

FeedManager::~FeedManager() {
  delete _feedAudio;
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void FeedManager::cancel() {
  this->clear();
  _feedAudio->stop();
  _dim();
}

void FeedManager::clear() {
  _arrayOfFeeds.clear();
}

void FeedManager::init() {
  _feedAudio = new Audio;
  _feedFont = fontManager.loadDefault();
}

bool FeedManager::hasQueued() {
  return !_arrayOfFeeds.empty();
}

bool FeedManager::isPlaying() {
  return _feedAudio->isPlaying();
}

void FeedManager::queue(const char* text, const char* audio) {
  if (_feedAudio->state() != kAudioPlaying) {
    this->showAndPlay(text, audio);
  }
  else {
    DGFeed feed;
    
    strncpy(feed.text, text, kMaxFeedLength);
    strncpy(feed.audio, audio, kMaxFileLength);
    
    _arrayOfFeeds.push_back(feed);
  }
}

void FeedManager::reshape() {
  std::vector<DGFeed>::iterator it;
  
  it = _arrayOfActiveFeeds.begin();
  
  while (it != _arrayOfActiveFeeds.end() && !_arrayOfActiveFeeds.empty()) {
    _calculatePosition(&(*it));
    
    ++it;
  }
}

// TODO: Note the font manager should purge unused fonts
void FeedManager::setFont(const char* fromFileName, unsigned int heightOfFont) {
  _feedFont = fontManager.load(fromFileName, heightOfFont);
  _feedHeight = heightOfFont;
}

void FeedManager::show(const char* text) {
  if (config.subtitles && (strcmp(text, "") != 0)) {
    std::string str = text;
    std::size_t maxChars = config.displayWidth / _feedHeight;
    std::size_t even = str.length() / (str.length() / maxChars + 1);
    std::size_t currSpace = 0;
    std::size_t nextSpace = 0;
    
    _dim();
    while (nextSpace != str.npos) {
      nextSpace = str.find(" " , even + currSpace);
      std::string substr = str.substr(currSpace, (nextSpace - currSpace));
      
      DGFeed feed;
      
      strncpy(feed.text, substr.c_str(), kMaxFeedLength);
      _calculatePosition(&feed);
      feed.color = kColorWhite - 0xFF000000;
      feed.state = DGFeedFadeIn;
      feed.timerHandle = timerManager.createManual(even * DGFeedSpeed); // Trigger should be configurable
      
      _arrayOfActiveFeeds.push_back(feed);
      
      currSpace = currSpace + substr.length() + 1;
    }
  }
}

void FeedManager::showAndPlay(const char* text, const char* audio) {
  this->show(text);

  if (!config.silentFeeds && strlen(audio) > 0) {
    audioManager._unregisterAudio(_feedAudio);
    delete _feedAudio;

    _feedAudio = new Audio;
    _feedAudio->setAudioName(audio);

    if (Control::instance().currentRoom()) {
      Control::instance().currentRoom()->claimAsset(_feedAudio);
    }
    else {
      Control::instance().assetRoom()->claimAsset(_feedAudio);
    }

    audioManager.registerAudio(_feedAudio);
    _feedAudio->play();
  }
}

void FeedManager::update() {
  std::vector<DGFeed>::iterator it;
  
  it = _arrayOfActiveFeeds.begin();
  
  while (it != _arrayOfActiveFeeds.end() && !_arrayOfActiveFeeds.empty()) {
    switch ((*it).state) {
      case DGFeedFadeIn:
        (*it).color += 0x08000000;
        if ((*it).color >= 0xEE000000) {
          // (*it).color = DGColorWhite;
          (*it).state = DGFeedIdle;
        }
        break;
      case DGFeedIdle:
        if (timerManager.checkManual((*it).timerHandle))
          (*it).state = DGFeedFadeOut;
        break;
      case DGFeedFadeOut:
        (*it).color -= 0x05000000;
        if ((*it).color <= 0x01000000) {
          (*it).state = DGFeedDiscard;
        }
        break;
      case DGFeedFadeOutSlow:
        (*it).color -= 0x02000000;
        if ((*it).color <= 0x01000000) {
          (*it).state = DGFeedDiscard;
        }
        break;
    }
    
    // Fadeout extra lines
    if (((_arrayOfActiveFeeds.end() - it) > DGFeedMaxLines)) {
      (*it).state = DGFeedDiscard;
    }
    
    if ((*it).state != DGFeedDiscard) {
      int displace = static_cast<int>((it - _arrayOfActiveFeeds.end() + 1) * (_feedHeight + DGFeedMargin));
      
      // Shadow code
      if (DGFeedShadowEnabled) {
        _feedFont->setColor(kColorBlack & (*it).color);
        _feedFont->wPrint((*it).location.x + DGFeedShadowDistance,
                         (*it).location.y + displace + DGFeedShadowDistance, (*it).text);
      }
      
      _feedFont->setColor((*it).color);
      _feedFont->wPrint((*it).location.x, (*it).location.y + displace, (*it).text);
    }
    
    ++it;
  }
  
  // Check for queued feeds
  if (_feedAudio->state() != kAudioPlaying) {
    if (!_arrayOfFeeds.empty()) {
      DGFeed feed = _arrayOfFeeds.front();
      
      this->showAndPlay(feed.text, feed.audio);
      _arrayOfFeeds.erase(_arrayOfFeeds.begin());
    }
  }
  
  _flush();
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void FeedManager::_calculatePosition(DGFeed* feed) {
  // We have no option but to estimate this
  int width = static_cast<int>(_feedHeight * 0.66f);
  int length = static_cast<int>(strlen(feed->text) * width);
  feed->location = MakePoint((config.displayWidth >> 1) - (length / 2),
                             config.displayHeight - _feedHeight - DGFeedMargin);
}

void FeedManager::_dim() {
  std::vector<DGFeed>::iterator it;
  
  it = _arrayOfActiveFeeds.begin();
  
  while (it != _arrayOfActiveFeeds.end() && !_arrayOfActiveFeeds.empty()) {
    if ((*it).state == DGFeedIdle)
      (*it).state = DGFeedFadeOutSlow;
    ++it;
  }
}

void FeedManager::_flush() {
  bool done = false;
  
  std::vector<DGFeed>::iterator it;
  
  while (!done) {
    done = true;
    
    it = _arrayOfActiveFeeds.begin();
    while (it != _arrayOfActiveFeeds.end() && !_arrayOfActiveFeeds.empty()) {
      if ((*it).state == DGFeedDiscard) {
        _arrayOfActiveFeeds.erase(it);
        done = false;
        break;
      }
      
      ++it;
    }
  }
}

}
