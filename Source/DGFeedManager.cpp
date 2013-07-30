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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGAudioManager.h"
#include "Config.h"
#include "DGFeedManager.h"
#include "DGFontManager.h"
#include "DGTimerManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGFeedManager::DGFeedManager() :
    audioManager(DGAudioManager::instance()),
    config(Config::instance()),
    fontManager(DGFontManager::instance()),
    timerManager(DGTimerManager::instance())
{
    _feedHeight = kDefFeedSize;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGFeedManager::~DGFeedManager() {
    // Nothing to do here (manager destroys the audio)
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGFeedManager::cancel() {
    this->clear();
    
    _feedAudio->stop();
    _dim();
}

void DGFeedManager::clear() {
    _arrayOfFeeds.clear();
}

void DGFeedManager::init() {
    _feedAudio = new DGAudio;
    _feedAudio->setStatic();
    audioManager.registerAudio(_feedAudio);
    
    _feedFont = fontManager.loadDefault();
}

bool DGFeedManager::hasQueued() {
    return !_arrayOfFeeds.empty();
}

bool DGFeedManager::isPlaying() {
    return _feedAudio->isPlaying();
}

void DGFeedManager::queue(const char* text, const char* audio) {
    if (_feedAudio->state() != DGAudioPlaying) {
        this->showAndPlay(text, audio);
    }
    else {
        DGFeed feed;
        
        strncpy(feed.text, text, kMaxFeedLength);
        strncpy(feed.audio, audio, kMaxFileLength);
        
        _arrayOfFeeds.push_back(feed);
    }
}

void DGFeedManager::reshape() {
    vector<DGFeed>::iterator it;
    
    it = _arrayOfActiveFeeds.begin();
    
    while (it != _arrayOfActiveFeeds.end() && !_arrayOfActiveFeeds.empty()) {
        _calculatePosition(&(*it));
        
        it++;
    }
}

// TODO: Note the font manager should purge unused fonts
void DGFeedManager::setFont(const char* fromFileName, unsigned int heightOfFont) {
    _feedFont = fontManager.load(fromFileName, heightOfFont);
    _feedHeight = heightOfFont;
}

void DGFeedManager::show(const char* text) {
    if (config.subtitles && (strcmp(text, "") != 0)) {
        string str = text;
        size_t maxChars = config.displayWidth / _feedHeight;
        size_t even = str.length() / (str.length() / maxChars + 1);
        size_t currSpace = 0;
        size_t nextSpace = 0;
        
        _dim();
        while (nextSpace != str.npos) {
            nextSpace = str.find(" " , even + currSpace);
            string substr = str.substr(currSpace, (nextSpace - currSpace));
            
            DGFeed feed;
            
            strncpy(feed.text, substr.c_str(), kMaxFeedLength);
            _calculatePosition(&feed);
            feed.color = kColorWhite - 0xFF000000;
            feed.state = DGFeedFadeIn;
            feed.timerHandle = timerManager.createManual((float)(even * DGFeedSpeed)); // Trigger should be configurable
            
            _arrayOfActiveFeeds.push_back(feed);
            
            currSpace = currSpace + substr.length() + 1;
        }
    }
}

void DGFeedManager::showAndPlay(const char* text, const char* audio) {
    this->show(text);
    
    if (_feedAudio->isLoaded())
        _feedAudio->unload();
    
    if (!config.silentFeeds) {
        _feedAudio->setResource(audio);
        audioManager.requestAudio(_feedAudio);
        _feedAudio->play();
    }
}

void DGFeedManager::update() {
    vector<DGFeed>::iterator it;
    
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
            long displace = (it - _arrayOfActiveFeeds.end() + 1) * (_feedHeight + DGFeedMargin);
            
            // Shadow code
            if (DGFeedShadowEnabled) {
                _feedFont->setColor(kColorBlack & (*it).color);
                _feedFont->print((*it).location.x + DGFeedShadowDistance,
                                 (*it).location.y + displace + DGFeedShadowDistance, (*it).text);
            }
            
            _feedFont->setColor((*it).color);
            _feedFont->print((*it).location.x, (*it).location.y + displace, (*it).text);
        }
        
        it++;
    }
    
    // Check for queued feeds
    if (_feedAudio->state() != DGAudioPlaying) {
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

void DGFeedManager::_calculatePosition(DGFeed* feed) {
    int width = (_feedHeight * 0.66f); // We have no option but to estimate this
    size_t length = strlen(feed->text) * width;
    
    feed->location.x = (config.displayWidth / 2) - (length / 2);
    feed->location.y = config.displayHeight - _feedHeight - DGFeedMargin;
}

void DGFeedManager::_dim() {
    vector<DGFeed>::iterator it;
    
    it = _arrayOfActiveFeeds.begin();
    
    while (it != _arrayOfActiveFeeds.end() && !_arrayOfActiveFeeds.empty()) {
        if ((*it).state == DGFeedIdle)
            (*it).state = DGFeedFadeOutSlow;
        it++;
    }
}

void DGFeedManager::_flush() {
    bool done = false;
    
    vector<DGFeed>::iterator it;
    
    while (!done) {
        done = true;
        
        it = _arrayOfActiveFeeds.begin();
        while (it != _arrayOfActiveFeeds.end() && !_arrayOfActiveFeeds.empty()) {
            if ((*it).state == DGFeedDiscard) {
                _arrayOfActiveFeeds.erase(it);
                done = false;
                break;
            }
            
            it++;
        }
    }
}
