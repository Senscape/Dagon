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
#include "DGTimerManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGFeedManager::DGFeedManager() {	
    config = &DGConfig::getInstance();
    timerManager = &DGTimerManager::getInstance();    
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

int DGFeedManager::color() {
    return (*it).color;
}

DGPoint DGFeedManager::location() {
    DGPoint location;
    location = (*it).location;
    location.y += (it - _arrayOfFeeds.end()) * DGDefFeedSize;
    return location;
}

bool DGFeedManager::isQueued() {
    return !_arrayOfFeeds.empty();
}

void DGFeedManager::parse(const char* text) {
    int length = (int)strlen(text);
    const int maxChars = config->displayWidth / DGDefFeedSize;
    
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
		
		feed.location.x = (config->displayWidth / 2) - ((even / 2) * DGDefFeedSize) + DGFeedMargin;
		feed.location.y = config->displayHeight - ((split + 1) * DGDefFeedSize) - DGFeedMargin;
	}
	else {
		feed.location.x = (config->displayWidth / 2) - ((length / 2) * DGDefFeedSize) + DGFeedMargin;
		feed.location.y = config->displayHeight - DGDefFeedSize - DGFeedMargin;
	}
	
    // FIXME: Must implement masks for colors
    feed.color = DGColorWhite - 0xFF000000;
    feed.state = DGFeedFadeIn;
	feed.timerHandle = timerManager->create((float)(length * DGFeedSpeed), 0); // Trigger should be configurable  

    _arrayOfFeeds.push_back(feed);
    
    it = _arrayOfFeeds.begin();
}

const char* DGFeedManager::text() {
    return (*it).text;    
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
                if (timerManager->check((*it).timerHandle))
                    (*it).state = DGFeedFadeOut;
                break;
            case DGFeedFadeOut:
                (*it).color -= 0x05000000;
                if ((*it).color <= 0x01000000)
                    (*it).state = DGFeedDiscard;
                break;
            case DGFeedDiscard:
                _arrayOfFeeds.erase(it);
                break;        
        }
        
        it++;
    }
}
