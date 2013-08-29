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

#ifndef DG_SPOT_H
#define DG_SPOT_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Action.h"
#include "Platform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum DGSpotFlags {
    DGSpotAuto = 0x1,
	DGSpotClass = 0x2,
	DGSpotLoop = 0x4,    
	DGSpotSync = 0x8,    
	DGSpotUser = 0x10
};

class Audio;
class DGTexture;
class DGVideo;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGSpot : public Object {
    int _color;
	int _flags;
    
	Action* _actionData;
  Audio* _attachedAudio;
    DGTexture* _attachedTexture;
	DGVideo* _attachedVideo;
	
	bool _isPlaying;
	
	bool _hasAction;
	bool _hasAudio;
	bool _hasColor; 
	bool _hasTexture;
	bool _hasVideo;
    
    float _volume;
	
    std::vector<int> _arrayOfCoordinates;
	unsigned int _onFace;
	
	int _xOrigin;
	int _yOrigin;
	int _zOrder; // For future use
    
public:
    DGSpot(std::vector<int> withArrayOfCoordinates, unsigned int onFace, int withFlags);
    ~DGSpot();
    
    // Checks
    
    bool hasAction();
    bool hasAudio();
    bool hasColor();
    bool hasFlag(int theFlag);
    bool hasTexture();
    bool hasVideo();
    bool isPlaying();
    
    // Gets
    
    Action* action();
    Audio* audio();
    int color();
    std::vector<int> arrayOfCoordinates();
    unsigned int face();
    Point origin();
    DGTexture* texture();
    int vertexCount();
    DGVideo* video();
    float volume();
    
    // Sets
    
    void setAction(Action* anAction);
    void setAudio(Audio* anAudio);
    void setColor(int aColor);
    void setOrigin(int x, int y);
    void setTexture(DGTexture* aTexture);
    void setVideo(DGVideo* aVideo);
    void setVolume(float theVolume);
    
    // State changes
    
    void play();
    void resize(int width, int height);
    void stop();    
};

#endif // DG_SPOT_H
