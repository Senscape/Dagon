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

#include "DGConfig.h"
#include "DGFeedManager.h"
#include "DGFontManager.h"
#include "DGTimerManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGFeedManager::DGFeedManager() {	
    config = &DGConfig::getInstance();
    fontManager = &DGFontManager::getInstance();    
    timerManager = &DGTimerManager::getInstance();   
    
    _feedHeight = DGDefFeedSize;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGFeedManager::~DGFeedManager() {
    // Nothing to do here
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGFeedManager::init() {
    _feedFont = fontManager->loadDefault();
}

bool DGFeedManager::isQueued() {
    return !_arrayOfFeeds.empty();
}

void DGFeedManager::parse(const char* text) {
    int length = (int)strlen(text);
    const int maxChars = config->displayWidth / _feedHeight;
    
    DGFeed feed;
    
	strncpy(feed.text, text, DGMaxFeedLength);
	
	if (length > maxChars) {
		char* p = feed.text;
		int split = length / maxChars;
		int even = length / (split + 1);
		
		for (int s = 0; s < split; s++) {
			for (int c = 0; c < even; c++)
				p++;
			
			while (*p != ' ')
				p++;
			
			*p = '\n';
		}
		
		feed.location.x = (config->displayWidth / 2) - ((even / 2) * _feedHeight) + DGFeedMargin;
		feed.location.y = config->displayHeight - ((split + 1) * _feedHeight) - DGFeedMargin;
	}
	else {
		feed.location.x = (config->displayWidth / 2) - ((length / 2) * _feedHeight) + DGFeedMargin;
		feed.location.y = config->displayHeight - _feedHeight - DGFeedMargin;
	}
	
    // FIXME: Must implement masks for colors
    feed.color = DGColorWhite - 0xFF000000;
    feed.state = DGFeedFadeIn;
	feed.timerHandle = timerManager->createManual((float)(length * DGFeedSpeed)); // Trigger should be configurable  

    _arrayOfFeeds.push_back(feed);
}

// TODO: Note the font manager should purge unused fonts
void DGFeedManager::setFont(const char* fromFileName, unsigned int heightOfFont) {
    _feedFont = fontManager->load(fromFileName, heightOfFont);
    _feedHeight = heightOfFont;
}

void DGFeedManager::update() {
    std::vector<DGFeed>::iterator it;
    
    it = _arrayOfFeeds.begin();
    
    while (it != _arrayOfFeeds.end() && !_arrayOfFeeds.empty()) {
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
                _arrayOfFeeds.erase(it);
                // If it was the last one, then stop everything
                if (it == _arrayOfFeeds.end())
                    return;                
                break;        
        }
        
        int displace = (it - _arrayOfFeeds.end() + 1) * (_feedHeight + DGFeedMargin);
        _feedFont->setColor((*it).color);
        _feedFont->print((*it).location.x, (*it).location.y + displace, (*it).text);
        
        it++;
    }
}
