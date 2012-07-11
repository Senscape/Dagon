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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGAudioManager.h"
#include "DGConfig.h"
#include "DGFeedManager.h"
#include "DGFontManager.h"
#include "DGTimerManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGFeedManager::DGFeedManager() {
    audioManager = &DGAudioManager::getInstance();  
    config = &DGConfig::getInstance();
    fontManager = &DGFontManager::getInstance();    
    timerManager = &DGTimerManager::getInstance();   
    
    _feedHeight = DGDefFeedSize;
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

void DGFeedManager::init() {
    _feedAudio = new DGAudio;
    _feedAudio->setStatic();
    audioManager->registerAudio(_feedAudio);
    
    _feedFont = fontManager->loadDefault();
}

bool DGFeedManager::hasQueued() {
    return !_arrayOfFeeds.empty();
}

void DGFeedManager::show(const char* text) {
    string str = text;
    size_t maxChars = config->displayWidth / _feedHeight;
    size_t even = str.length() / (str.length() / maxChars + 1);
    size_t currSpace = 0;
    size_t nextSpace = 0; 
    
    while (nextSpace != str.npos) {
        nextSpace = str.find(" " , even + currSpace);
        string substr = str.substr(currSpace, (nextSpace - currSpace));
        
        DGFeed feed;
        int width = (_feedHeight * 0.66f); // We have no option but to estimate this
        
        strncpy(feed.text, substr.c_str(), DGMaxFeedLength);
        feed.location.x = (config->displayWidth / 2) - ((substr.length() / 2) * width);
		feed.location.y = config->displayHeight - _feedHeight - DGFeedMargin;
        feed.color = DGColorWhite - 0xFF000000;
        feed.state = DGFeedFadeIn;
        feed.timerHandle = timerManager->createManual((float)(even * DGFeedSpeed)); // Trigger should be configurable  
        
        _arrayOfActiveFeeds.push_back(feed);
        
        currSpace = currSpace + substr.length() + 1;
    }
}

void DGFeedManager::showAndPlay(const char* text, const char* audio) {
    this->show(text);
    
    if (_feedAudio->isLoaded())
        _feedAudio->unload();
    
    _feedAudio->setResource(audio);
    audioManager->requestAudio(_feedAudio);
    _feedAudio->play();
}

void DGFeedManager::queue(const char* text, const char* audio) {
    if (_feedAudio->state() != DGAudioPlaying) {
        this->showAndPlay(text, audio);
    }
    else {
        DGFeed feed;
        
        strncpy(feed.text, text, DGMaxFeedLength);
        strncpy(feed.audio, audio, DGMaxFileLength);
        
        _arrayOfFeeds.push_back(feed);
    }
}

// TODO: Note the font manager should purge unused fonts
void DGFeedManager::setFont(const char* fromFileName, unsigned int heightOfFont) {
    _feedFont = fontManager->load(fromFileName, heightOfFont);
    _feedHeight = heightOfFont;
}

void DGFeedManager::update() {
    vector<DGFeed>::iterator it;
    
    it = _arrayOfActiveFeeds.begin();
    
    while (it != _arrayOfActiveFeeds.end() && !_arrayOfActiveFeeds.empty()) {
        switch ((*it).state) {
            case DGFeedFadeIn:
                (*it).color += 0x10000000;
                if ((*it).color >= 0xEE000000) {
                    (*it).color = DGColorWhite;
                    (*it).state = DGFeedIdle;
                }
            case DGFeedIdle:
                if (timerManager->checkManual((*it).timerHandle))
                    (*it).state = DGFeedFadeOut;
                break;
            case DGFeedFadeOut:
                (*it).color -= 0x05000000;
                if ((*it).color <= 0x01000000)
                    (*it).state = DGFeedDiscard;
                break;
            case DGFeedDiscard:
                _arrayOfActiveFeeds.erase(it);
                // If it was the last one, then stop everything
                if (it == _arrayOfActiveFeeds.end())
                    return;                
                break;        
        }
        
        long displace = (it - _arrayOfActiveFeeds.end() + 1) * (_feedHeight + DGFeedMargin);
        _feedFont->setColor((*it).color);
        _feedFont->print((*it).location.x, (*it).location.y + displace, (*it).text);
        
        // Fadeout extra lines
        if (((_arrayOfActiveFeeds.end() - it) > DGFeedMaxLines) && (*it).state == DGFeedIdle) {
            (*it).state = DGFeedFadeOut;
        }
        
        it++;
    }
    
    // Check for queued feeds
    if (_feedAudio->state() != DGAudioPlaying) {
        if (!_arrayOfFeeds.empty()) {
            DGFeed feed = _arrayOfFeeds.back();
            
            this->showAndPlay(feed.text, feed.audio);
            _arrayOfFeeds.pop_back();
        }
    } 
}
